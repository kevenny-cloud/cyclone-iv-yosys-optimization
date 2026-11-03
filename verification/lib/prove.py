"""Independently check native graphs with OSS Yosys and EQY SAT partitions.

No source or submission package is changed.  EQY setup is used because this
portable Windows installation does not include make/sh; generated run.ys files
are executed directly with the independent Yosys executable.
"""
import sys
sys.dont_write_bytecode = True
from pathlib import Path
import collections
import hashlib
import importlib.util
import inspect
import json
import os
import re
import shutil
import subprocess

ROOT = Path(__file__).resolve().parent
PROJECT = ROOT.parents[1]
SUITE = PROJECT.parent / 'oss-cad-suite'
YOSYS = SUITE / 'bin/yosys.exe'
MODEL = PROJECT / '提交材料/verification/model/primitive_decode.v'

def read(p): return json.loads(p.read_text(encoding='utf-8-sig'))
def write(p, value): p.write_text(json.dumps(value, ensure_ascii=False, indent=2)+'\n', encoding='utf-8')
def sha(p): return hashlib.sha256(p.read_bytes()).hexdigest()

def guard(path, top, allow_sync=False, allow_load_data=False):
    graph=read(path)['modules'][top]
    cells=graph['cells']
    used={b for p in graph['ports'].values() if p['direction']=='output' for b in p['bits']}
    for c in cells.values():
        for p,d in c['port_directions'].items():
            if d=='input': used.update(c['connections'][p])
    counts=collections.Counter()
    for name,c in cells.items():
        counts[c['type']]+=1
        if c['type']=='dffeas':
            assert c['parameters']['power_up'] in ('low','high'), (path,name,'power_up')
            fixed={'prn':'1','aload':'0'}if allow_sync else {'prn':'1','asdata':'0','aload':'0','sclr':'0','sload':'0'}
            for p,val in fixed.items():
                assert c['connections'][p]==[val], (path,name,p,c['connections'][p])
            if allow_sync:
                if allow_load_data:
                    assert len(c['connections']['asdata'])==1 and all(isinstance(b,int)or b in ('0','1')for b in c['connections']['asdata']),(path,name,'binary asdata required')
                else:
                    assert c['connections']['asdata'] in (['0'],['1']),(path,name,'asdata must be constant')
                assert len(c['connections']['sclr'])==len(c['connections']['sload'])==1
            assert len(c['connections']['clk'])==len(c['connections']['clrn'])==len(c['connections']['ena'])==1
        elif c['type']=='cycloneiv_lcell_comb':
            assert c['parameters'].get('sum_lutc_input','datac')=='datac', (path,name,'sum_lutc_input')
            assert not (set(c['connections'].get('cout',[])) & used), (path,name,'used cout')
        else:
            assert c['type'] in ('$not','$lut'), (path,name,c['type'])
    return {'status':'PASS','sha256':sha(path),'cell_types':dict(counts),
            'dffeas_fixed_controls':(('prn=1; aload=0; binary dynamic asdata; ENA>SCLR>SLOAD; power_up low/high'if allow_load_data else'prn=1; aload=0; binary constant asdata; ENA>SCLR>SLOAD; power_up low/high')if allow_sync else'prn=1; asdata=aload=sclr=sload=0; power_up low/high'),
            'lcell_restrictions':'sum_lutc_input=datac; cout unobserved'}

def execute(args, cwd, logfile, env, timeout=120):
    with logfile.open('wb') as stream:
        p=subprocess.run(args,cwd=cwd,env=env,stdout=stream,stderr=subprocess.STDOUT,
                         timeout=timeout,creationflags=subprocess.CREATE_NO_WINDOW)
    if p.returncode: raise RuntimeError((str(logfile),p.returncode))

def eqy_setup():
    # Load the retained EQY application with Windows CPython, avoiding its
    # launcher which mishandles the non-ASCII absolute installation path.
    sys.path.insert(0,str(SUITE/'lib/python3.11/site-packages'))
    sys.path.insert(0,str(SUITE/'share/yosys/python3'))
    spec=importlib.util.spec_from_file_location('native_eqy',SUITE/'bin/eqy-script.py')
    module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
    def native_root_path():
        # Byte-identical plugin copies live beside proof.eqy.  The partition
        # subprocess alone changes cwd into its workdir before loading one.
        return '..' if inspect.currentframe().f_back.f_code.co_name=='make_partitions' else '.'
    module.root_path=native_root_path
    sys.argv=['eqy','-m','-d','eqy-partitions','proof.eqy']
    module.main()

def main():
    global MODEL
    env={k:v for k,v in os.environ.items() if not k.startswith('YOSYS') and k not in ('ABC','FPGA_DIAG_FILE')}
    env['PATH']=str(SUITE/'bin')+os.pathsep+str(SUITE/'lib')+os.pathsep+env['PATH']
    env['PYTHONUTF8']='1'
    import argparse
    ap=argparse.ArgumentParser();ap.add_argument('--matrix',default='matrix-v3');ap.add_argument('--depth',type=int,default=20)
    ap.add_argument('--case',action='append');ap.add_argument('--resume',action='store_true')
    ap.add_argument('--state-relation',action='store_true',help='Prove a complete bijective register relation by independent combinational SAT, including every clock, async-clear response, transition, initial value and public output')
    ap.add_argument('--sync-load-data',action='store_true',help='Explicitly permit binary dynamic ASDATA in the separately supplied synchronous-control model')
    ap.add_argument('--model',type=Path);ap.add_argument('--sync-controls',action='store_true');ap.add_argument('--short-dirs',action='store_true',help='Short proof directories avoid native Windows path length limits');ap.add_argument('--clock-history',action='store_true',help='Publish and prove canonical clock/state sample history names after clk2fflogic');ap.add_argument('--split-state',action='store_true',help='Keep separate EQY state partitions; every partition must prove');ap.add_argument('--preserve-names',action='store_true',help='Retain internal signal names and prove all matched internal assertions as well as public outputs');ap.add_argument('--multiclock',action='store_true',help='Sample all real clock edges with clk2fflogic before EQY partitioning');args=ap.parse_args()
    if args.model:MODEL=args.model.resolve()
    assert not args.sync_controls or args.model,'Explicit separately verified hardware model required for sync controls'
    assert not args.sync_load_data or(args.sync_controls and args.model),'Dynamic ASDATA requires explicit synchronous-control model'
    assert not args.clock_history or(args.multiclock and args.preserve_names)
    assert not args.state_relation or(args.multiclock and not args.clock_history),'State-relation mode uses explicit clock equations directly'
    assert 1<=args.depth<=200
    matrix=ROOT/args.matrix; source=read(matrix/'summary.json')
    assert source['status']=='EXPORTED', 'Complete all resource exports before proving'
    previous=None;revision='proof_driver.py'
    if args.resume:
        previous=read(matrix/'proof_summary.json')
        assert previous['candidate_sha256']==source['config']['candidate']['yosys_sha256']
        history=matrix/'proof-history';history.mkdir(exist_ok=True)
        number=1
        while (history/('summary-'+str(number)+'.json')).exists():number+=1
        shutil.copy2(matrix/'proof_summary.json',history/('summary-'+str(number)+'.json'))
        revision='proof_driver-resume-'+str(number)+'.py'
    shutil.copy2(Path(__file__).resolve(),matrix/revision)
    report={'status':'RUNNING','scope':'Independent EQY partitions and direct generated SAT temporal induction scripts; discrete synchronous semantics with defined formal inputs',
            'proof_tool':str(YOSYS),'proof_tool_sha256':sha(YOSYS),
            'eqy_script_sha256':sha(SUITE/'bin/eqy-script.py'),
            'proof_driver_sha256':sha(matrix/'proof_driver.py'),
            'model_sha256':sha(MODEL),'candidate_sha256':source['config']['candidate']['yosys_sha256'],
            'model':str(MODEL),'sync_controls':args.sync_controls,'sync_load_data':args.sync_load_data,
            'boundaries':{
                'driver':'EQY setup only; generated SAT scripts run directly; no complete EQY make driver PASS',
                'inputs':'defined formal inputs; not arbitrary four-state X/Z simulation',
                'async_reset':'async2sync synchronous sampling and negative-hold-time assumption; case04 has asynchronous reset',
                'initial_state':'actual dffeas power_up low/high decoded as discrete initial state',
                'objects':'baseline versus candidate mapped graphs, not separate RTL-to-mapped end-to-end proof',
                'not_covered':'device timing, asynchronous event phasing, glitches, recovery/removal timing, analog power-up and power'},
            'cases':{}}
    version=subprocess.run([str(YOSYS),'-V'],env=env,stdout=subprocess.PIPE,check=True).stdout.decode('utf-8',errors='replace').strip()
    report['proof_tool_version']=version
    if previous:
        assert previous['proof_tool_sha256']==report['proof_tool_sha256']and previous['model_sha256']==report['model_sha256']
        report=previous;report['status']='RUNNING';report.pop('error',None)
        report.setdefault('proof_driver_revisions',[]).append({'file':revision,'sha256':sha(matrix/revision),'depth':args.depth})
    try:
        # Exercise state preservation and newly rewritten feedback designs first.
        names=sorted(source['cases'],key=lambda n:(0 if n=='test3' else 1 if n.startswith('pmux_case') else 2 if n.startswith('epfl') else 3,n))
        if args.case:
            assert set(args.case)<=set(names)
            names=[n for n in names if n in args.case]
        for name in names:
            if args.resume and not args.case and report['cases'].get(name,{}).get('status')=='PASS':continue
            case=source['cases'][name]
            folder=matrix/name; top=case['top']; result={'checkpoints':{},'driver_revision':revision,'induction_depth':args.depth,'clock_semantics':'clk2fflogic sampled arbitrary clock edges'if args.multiclock else'implicit synchronous steps and async2sync','preserved_internal_names':args.preserve_names,'split_state_partitions':args.split_state};report['cases'][name]=result
            if args.state_relation:
                result['clock_semantics']='Complete register relation with arbitrary binary clock/reset events; all clocks, reset responses, transitions, initial values and public outputs proved'
            for checkpoint,label in [('checkpoint','abc'),('final','final')]:
                proof_prefix=('pa'if label=='abc'else'pf')if args.short_dirs else'independent-proof-'+label
                proof=folder/proof_prefix
                attempt=0
                while proof.exists():
                    attempt+=1;proof=folder/(proof_prefix+'-r'+str(attempt))
                proof.mkdir()
                shutil.copy2(MODEL,proof/'primitive_decode.v')
                record={'status':'RUNNING','directory':str(proof),'guards':{},'partitions':{},'transport':{},'decoded':{}};result['checkpoints'][label]=record
                record['plugin_sha256']={}
                for plugin in ['eqy_combine.so','eqy_partition.so','eqy_recode.so']:
                    shutil.copy2(SUITE/'share/yosys/plugins'/plugin,proof/plugin)
                    record['plugin_sha256'][plugin]=sha(proof/plugin)
                    assert sha(SUITE/'share/yosys/plugins'/plugin)==record['plugin_sha256'][plugin]
                retained=[]
                if args.preserve_names:
                    graphs=[read(folder/v/(checkpoint+'.json'))['modules'][top]for v in ('baseline','candidate')]
                    occupied=[set(),set()]
                    source_identifiers=set()
                    for input_name in case['input_sha256']:
                        text=(folder/input_name).read_text(encoding='utf-8',errors='replace')
                        source_identifiers.update(re.findall(r'[A-Za-z_][A-Za-z0-9_]*',text))
                    common=set(graphs[0]['netnames'])&set(graphs[1]['netnames'])&source_identifiers
                    for netname in sorted(common):
                        if not re.fullmatch(r'[A-Za-z_][A-Za-z0-9_]*',netname):continue
                        nets=[g['netnames'][netname]for g in graphs]
                        if len(nets[0]['bits'])!=len(nets[1]['bits']):continue
                        bits=[set(n['bits'])for n in nets]
                        if any(not all(isinstance(b,int)for b in x)or x&used for x,used in zip(bits,occupied)):continue
                        retained.append(netname)
                        for x,used in zip(bits,occupied):used.update(x)
                record['retained_internal_names']=retained
                for variant in ('baseline','candidate'):
                    raw=folder/variant/(checkpoint+'.il')
                    normalized=proof/(variant+'.il')
                    normalized.write_bytes(raw.read_bytes().replace(b'\r\n',b'\n'))
                    record['transport'][variant]={'raw_sha256':sha(raw),'lf_sha256':sha(normalized)}
                    record['guards'][variant]=guard(folder/variant/(checkpoint+'.json'),top,args.sync_controls,args.sync_load_data)
                    decode_script=proof/(variant+'-decode.ys')
                    hide_names=('rename -hide * '+''.join('w:'+n+' %d 'for n in retained)+'\n')if args.preserve_names else'rename -hide\n'
                    decode_script.write_text(f'read_rtlil {variant}.il\ntechmap -map primitive_decode.v\nopt\nprep -top {top}\nselect -module {top}\n{hide_names}write_rtlil -selected {variant}-generic.il\n',encoding='utf-8')
                    execute([str(YOSYS),'-Q','-T','-s',decode_script.name],proof,proof/(variant+'-decode.log'),env)
                    record['decoded'][variant]={'generic_sha256':sha(proof/(variant+'-generic.il')),'script_sha256':sha(decode_script),'log_sha256':sha(proof/(variant+'-decode.log'))}
                clock_files={v:v+'-generic.il'for v in ('baseline','candidate')}
                if args.clock_history:
                    record['clock_history']={}
                    for variant in ('baseline','candidate'):
                        prep=proof/(variant+'-clock.ys')
                        prep.write_text(f'read_rtlil {variant}-generic.il\nclk2fflogic\nopt\nwrite_rtlil {variant}-clock.il\n',encoding='utf-8')
                        execute([str(YOSYS),'-Q','-T','-s',prep.name],proof,proof/(variant+'-clock.log'),env)
                        before=proof/(variant+'-clock.il');text=before.read_text(encoding='utf-8');renames={};taken=set()
                        for line in text.splitlines():
                            if not line.lstrip().startswith('wire '):continue
                            old=line.split()[-1]
                            match=re.search(r'sample_(data|control_edge)\$\\([A-Za-z_][A-Za-z0-9_]*(?:\[[0-9]+\])?)#sampled\$[0-9]+$',old)
                            if not match:continue
                            kind,sig=match.groups();base=re.sub(r'\[.*','',sig)
                            if base not in retained:continue
                            new=chr(92)+'ph'+kind[0]+hashlib.sha256(sig.encode()).hexdigest()[:8]
                            if new in taken:continue
                            assert new not in text,'Proof hint name collision'
                            taken.add(new);renames[old]=new
                        for old,new in renames.items():
                            text=re.sub(r'(?<!\S)'+re.escape(old)+r'(?=\s|$)',lambda m:new,text)
                        after=proof/(variant+'-clock-named.il');after.write_text(text,encoding='utf-8')
                        record['clock_history'][variant]={'before':before.name,'before_sha256':sha(before),'after':after.name,'after_sha256':sha(after),
                            'script':prep.name,'script_sha256':sha(prep),'log_sha256':sha(proof/(variant+'-clock.log')),'identifier_bijection':renames}
                        clock_files[variant]=after.name
                def semantic_text(path):
                    # Only provenance and allocator-counter metadata are ignored.
                    # All cells, connectivity, values, init/keep and other attrs remain.
                    return '\n'.join(line for line in path.read_text(encoding='utf-8').splitlines()
                        if not line.lstrip().startswith(('#','autoidx ','attribute \\src ','attribute \\generator ')))
                if semantic_text(proof/'baseline-generic.il')==semantic_text(proof/'candidate-generic.il'):
                    record['status']='STRUCTURAL_IDENTITY'
                    record['identity_scope']='Decoded RTLIL exactly identical excluding comments, src/generator and autoidx only'
                    write(proof/'proof-result.json',record);write(matrix/'proof_summary.json',report)
                    print(name,label,record['status'],flush=True)
                    continue
                if args.state_relation:
                    import prove_state_relation
                    graphs=[read(folder/v/(checkpoint+'.json'))['modules'][top]for v in ('baseline','candidate')]
                    state=prove_state_relation.prove(*graphs,proof/'state-relation')
                    assert state['status']=='STATE_TRANSITION_SAT_PASS',(name,label,'Unproved complete state relation')
                    record['status']='STATE_TRANSITION_SAT_PASS'
                    record['state_relation']={'directory':'state-relation','summary_sha256':sha(proof/'state-relation/summary.json')}
                    write(proof/'proof-result.json',record);write(matrix/'proof_summary.json',report)
                    print(name,label,record['status'],state['registers'],'registers',flush=True)
                    continue
                clock_prep='\nclk2fflogic'if args.multiclock and not args.clock_history else''
                collect='solo *\n'if args.split_state else'bind *\ngroup *\n'
                (proof/'proof.eqy').write_text(f"[gold]\nread_rtlil {clock_files['baseline']}\nprep -top {top}{clock_prep}\n[gate]\nread_rtlil {clock_files['candidate']}\nprep -top {top}{clock_prep}\n[collect *]\n{collect}[strategy induction]\nuse sat\ndepth {args.depth}\ntimeout 90\n",encoding='utf-8')
                execute([sys.executable,str(Path(__file__).resolve()),'--eqy-setup'],proof,proof/'eqy-setup.log',env)
                run_scripts=sorted((proof/'eqy-partitions/strategies').glob('*/induction/run.ys'))
                assert run_scripts, (name,label,'No SAT partitions')
                for ys in run_scripts:
                    log=ys.parent/'independent-run.log'
                    execute([str(YOSYS),'-Q','-T','-s','run.ys'],ys.parent,log,env,120)
                    text=log.read_text(encoding='utf-8',errors='replace')
                    passed=('Induction step proven: SUCCESS!' in text or 'SAT proof finished - no model found: SUCCESS!' in text)
                    partition_name=ys.parent.parent.name
                    partition_model=proof/'eqy-partitions/partitions'/(partition_name+'.il')
                    record['partitions'][partition_name]={'status':'PASS' if passed else 'UNPROVEN','script_sha256':sha(ys),'log_sha256':sha(log),'model_sha256':sha(partition_model)}
                    assert passed,(name,label,str(log))
                record['status']='EQY_PARTITION_SAT_PASS'
                write(proof/'proof-result.json',record)
                write(matrix/'proof_summary.json',report)
                print(name,label,record['status'],len(run_scripts),'partitions',flush=True)
            result['status']='PASS'
        report['status']='PASS'if set(report['cases'])==set(source['cases'])and all(c.get('status')=='PASS'for c in report['cases'].values())else'INCOMPLETE'
    except Exception as exc:
        report['status']='INCOMPLETE';report['error']=repr(exc);raise
    finally:
        write(matrix/'proof_summary.json',report)

if __name__=='__main__':
    if '--eqy-setup' in sys.argv: eqy_setup()
    else: main()
