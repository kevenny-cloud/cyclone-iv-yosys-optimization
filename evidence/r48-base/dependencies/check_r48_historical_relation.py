"""Audit the complete relation and require rejection of deliberate graph faults."""
import copy,json,os,subprocess
from pathlib import Path
import prove_r48_historical_relation as proof
R=Path(__file__).resolve().parent
OUT=R.parents[2]/'p48relation-historical-checks'
OUT.mkdir(exist_ok=True)
record=json.loads((proof.OUT/'summary.json').read_text())
assert record['status']=='PASS'
env=dict(os.environ);env['PATH']=str(proof.guards.SUITE/'bin')+os.pathsep+str(proof.guards.SUITE/'lib')+os.pathsep+env['PATH']
results={'status':'RUNNING','audits':{},'mutations':{}}
for checkpoint in ('checkpoint','final'):
    report=record['checkpoints'][checkpoint]
    paths=[proof.M/v/(checkpoint+'.json') for v in ('baseline','candidate')]
    assert [proof.sha(p) for p in paths]==report['input_hashes']
    a,b=[json.loads(p.read_text())['modules'][proof.TOP] for p in paths]
    pairs={q:(r,p) for q,r,p in report['pairs']};fa,fb=proof.base.registers(a),proof.base.registers(b)
    assert set(pairs)==set(fa) and {r for r,p in pairs.values()}==set(fb)
    shape=lambda g:{n:(x['direction'],len(x['bits'])) for n,x in g['ports'].items()}
    assert shape(a)==shape(b)
    assert all((fa[q][1]['parameters']['power_up']=='high')==((fb[r][1]['parameters']['power_up']=='high')^bool(p)) for q,(r,p) in pairs.items())
    order=sorted(fa);idx={r:i for i,r in enumerate(sorted(fb))}
    sa={q:(idx[r],p) for q,(r,p) in pairs.items()};sb={r:(idx[r],0) for r in fb}
    text=proof.emit(a,'gold',sa,order,len(fb))+proof.emit(b,'gate',sb,[pairs[q][0] for q in order],len(fb))
    assert (proof.OUT/checkpoint/'relation.v').read_text()==text
    assert all(proof.sha(proof.OUT/checkpoint/n)==h for n,h in report['proof_hashes'].items())
    assert 'SAT proof finished - no model found: SUCCESS!' in (proof.OUT/checkpoint/'run.log').read_text()
    results['audits'][checkpoint]='PASS: all initial values, registers, interfaces, emitted equations and proof hashes verified'
    if checkpoint!='final':continue
    target=b['ports']['signature']['bits'][3]
    cellname=next(n for n,c in b['cells'].items() if c['type']=='dffeas' and c['connections']['q']==[target])
    broken=copy.deepcopy(b);c=broken['cells'][cellname];c['parameters']['power_up']='low' if c['parameters']['power_up']=='high' else 'high'
    assert any((fa[q][1]['parameters']['power_up']=='high')!=((proof.base.registers(broken)[r][1]['parameters']['power_up']=='high')^bool(p)) for q,(r,p) in pairs.items())
    results['mutations']['power_up']='REJECTED_INITIAL_RELATION'
    badpairs=dict(pairs);badpairs.pop(next(iter(badpairs)))
    assert set(badpairs)!=set(fa)
    results['mutations']['omitted_register']='REJECTED_INCOMPLETE_COVERAGE'
    for kind in ('clock','async_clear','synchronous_load','public_output'):
        broken=copy.deepcopy(b);c=broken['cells'][cellname]
        if kind=='clock':c['connections']['clk']=broken['ports']['en']['bits']
        elif kind=='async_clear':c['connections']['clrn']=['1']
        elif kind=='synchronous_load':c['connections']['sload']=['1']
        else:broken['ports']['signature']['bits'][3]='0'
        f=OUT/kind;f.mkdir(exist_ok=True)
        text=proof.emit(a,'gold',sa,order,len(fb))+proof.emit(broken,'gate',sb,[pairs[q][0] for q in order],len(fb))
        (f/'relation.v').write_text(text)
        script='read_verilog relation.v\nmiter -equiv -flatten gold gate miter\nprep -top miter\nsat -verify -timeout 30 -prove trigger 0 -set-def-inputs miter\n'
        (f/'run.ys').write_text(script)
        with (f/'run.log').open('wb') as log:p=subprocess.run([str(proof.guards.YOSYS),'-Q','-T','-s','run.ys'],cwd=f,env=env,stdout=log,stderr=subprocess.STDOUT,timeout=60)
        s=(f/'run.log').read_text(errors='replace')
        assert p.returncode!=0 and ('proof did fail' in s or 'model found: FAIL!' in s),(kind,s[-400:])
        results['mutations'][kind]='REJECTED_BY_SAT'
        print(kind,'rejected',flush=True)
results['status']='PASS'
(OUT/'summary.json').write_text(json.dumps(results,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
print('Audited both checkpoints; six deliberate errors rejected.',flush=True)
