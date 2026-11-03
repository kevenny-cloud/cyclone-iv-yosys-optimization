"""Paired isolated mapping diagnostic; excludes prior synthesis, never scores."""
from pathlib import Path
import json,subprocess,os,time,psutil,shutil,collections,re
R=Path(__file__).resolve().parent;D=R/'continuation-20261030/next-plan-execution'
O=D/'isolated-mapping-v3';assert not O.exists();O.mkdir()
exe=R/'continuation-20261030/r48-one-snapshot-clean/program/yosys.exe'
env={k:v for k,v in os.environ.items()if not k.startswith('YOSYS')and k!='ABC'}
env['PATH']=str(exe.parent)+os.pathsep+env.get('SystemRoot','C:/Windows')+'/System32'
report={'status':'ISOLATED_DIAGNOSTIC_NOT_SCORE','cases':{}}
for d in sorted((D/'permutation-experiment-v1').iterdir()):
 if not d.is_dir():continue
 report['cases'][d.name]={}
 for variant in ['original','canonical']:
  w=O/d.name/variant;w.mkdir(parents=True)
  src=D/'memory-diagnostic-v1'/d.name/'candidate/checkpoint.json' if variant=='original'else d/'candidate/canonical.json'
  il=(D/'memory-diagnostic-v1'/d.name/'candidate/checkpoint.il').read_text(encoding='utf-8')
  if variant=='canonical':
   module=json.loads(src.read_text())['modules'][d.name];bits={}
   for name,net in module['netnames'].items():
    for i,bit in enumerate(net['bits']):
     if isinstance(bit,int)and bit not in bits:bits[bit]='\\'+name+(' ['+str(i)+']' if len(net['bits'])>1 else '')
   def edit(match):
    block=match.group(0);name=match.group(1);cell=module['cells'][name]
    if cell['type']!='$lut':return block
    val=cell['parameters']['LUT'];pins=cell['connections']['A']
    block=re.sub(r'parameter \\LUT .*',lambda m:'parameter \\LUT '+str(len(val))+"'"+val,block)
    sig=' '.join(bits[b]if isinstance(b,int)else "1'"+b for b in reversed(pins))
    return re.sub(r'connect \\A .*',lambda m:'connect \\A { '+sig+' }',block)
   il=re.sub(re.escape('  cell $lut \\')+r'([^\n]+)\n.*?  end',edit,il,flags=re.S)
  (w/'input.il').write_text(il,encoding='utf-8')
  (w/'run.ys').write_text('read_verilog -lib +/intel/cycloneiv/cells_sim.v\nread_rtlil input.il\ntechmap -map +/intel/cycloneiv/cells_map.v\nclean -purge\ncheck -noinit -assert\nwrite_json final.json\n',encoding='utf-8')
  samples=[]
  with (w/'run.log').open('wb')as log:
   proc=subprocess.Popen([str(exe),'-Q','-s','run.ys'],cwd=w,env=env,stdout=log,stderr=subprocess.STDOUT);obs=psutil.Process(proc.pid)
   while proc.poll()is None:
    try:samples.append(obs.memory_info().peak_wset)
    except psutil.NoSuchProcess:break
    time.sleep(.001)
   assert proc.wait()==0
  cells=json.loads((w/'final.json').read_text())['modules'][d.name]['cells'];types=collections.Counter(c['type']for c in cells.values())
  report['cases'][d.name][variant]={'observed_peak_bytes':max(samples),'logic':types['cycloneiv_lcell_comb']+types['$not'],'dff':types['dffeas']}
 a,b=[report['cases'][d.name][v]for v in ['original','canonical']];assert(a['logic'],a['dff'])==(b['logic'],b['dff'])
 print(d.name,a,b,flush=True)
 (O/'summary.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
