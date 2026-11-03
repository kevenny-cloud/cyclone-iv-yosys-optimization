"""Re-export current executable, regenerate state equations, and prove RTL cases."""
from pathlib import Path
import argparse,hashlib,importlib,json,os,shutil,subprocess,sys
import sys as _sys
_sys.dont_write_bytecode=True  # never write .pyc into the package
ROOT=Path(__file__).resolve().parent
def main():
 ap=argparse.ArgumentParser();ap.add_argument('--yosys',type=Path,required=True,help='Independent OSS-CAD-Suite bin/yosys.exe');ap.add_argument('--out',type=Path,required=True);args=ap.parse_args()
 tool=args.yosys.resolve();out=args.out.resolve();assert not out.exists()and ROOT not in out.parents;out.mkdir(parents=True)
 sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();contract=json.loads((ROOT/'verification/functional-contract.json').read_text());assert sha(tool)==contract['independent_yosys_sha256'],'Pinned independent proof tool mismatch'
 env=dict(os.environ);env['PATH']=str(tool.parent)+os.pathsep+str(tool.parent.parent/'lib')+os.pathsep+env.get('PATH','');env['PYTHONUTF8']='1'
 subprocess.run([sys.executable,str(ROOT/'verify_portable.py'),'--out',str(out/'exports')],env=env,check=True)
 sys.path.insert(0,str(ROOT/'verification/lib'));import prove_state_relation as relation;import prove as guard;import many_to_one as many;import audit_complete_relation as relation_audit
 relation.YOSYS=tool;guard.YOSYS=tool;guard.SUITE=tool.parent.parent
 report={'status':'RUNNING','candidate_sha256':sha(ROOT/'yosys.exe'),'proof_tool_sha256':sha(tool),'cases':{},'pressure':{},'rtl':{},'scope':'R48-to-candidate exact state relations plus both original pressure references; explicit-init RTL collection; defined binary primitive semantics'}
 def save():(out/'summary.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
 def execute(exe,w,script,label,expected_fail=False):
  (w/(label+'.ys')).write_text(script,encoding='utf-8')
  with(w/(label+'.log')).open('wb')as f:p=subprocess.run([str(exe),'-Q','-s',label+'.ys'],cwd=w,env=env,stdout=f,stderr=subprocess.STDOUT,timeout=120)
  text=(w/(label+'.log')).read_text(encoding='utf-8',errors='replace')
  if expected_fail:assert p.returncode!=0 and ('proof did fail'in text or 'model found: FAIL'in text)
  else:assert p.returncode==0,(w,label)
  return text
 try:
  fixtures=json.loads((ROOT/'verification/fixtures.json').read_text())['cases']
  (out/'relations').mkdir()
  for name,c in fixtures.items():
   w=out/'relations'/name;w.mkdir();report['cases'][name]={}
   for checkpoint in ['checkpoint','final']:
    a=json.loads((ROOT/'verification/r48-graphs'/name/(checkpoint+'.json')).read_text())['modules'][c['top']]
    b=json.loads((out/'exports'/name/(checkpoint+'.json')).read_text())['modules'][c['top']];proof=w/checkpoint
    if relation.registers(a):
     result=relation.prove(a,b,proof);assert result['status']=='STATE_TRANSITION_SAT_PASS';relation.audit(a,b,proof);status=result['status']
    else:
     proof.mkdir();ga,n=relation.emit(a,'gold',{});gb,m=relation.emit(b,'gate',{});assert n==m;(proof/'relation.v').write_text(ga+gb)
     text=execute(tool,proof,'read_verilog relation.v\nmiter -equiv -flatten gold gate miter\nprep -top miter\nsat -verify -prove trigger 0 -set-def-inputs miter\n','proof');assert 'SAT proof finished - no model found: SUCCESS!'in text;status='COMBINATIONAL_SAT_PASS'
    report['cases'][name][checkpoint]=status;save()
   print(name,'STATE_OR_COMBINATIONAL_PASS',flush=True)
  (out/'pressure').mkdir()
  for reference in ['current','historical']:
   w=out/'pressure'/reference;w.mkdir();inputs=w/'graphs';inputs.mkdir();(inputs/'baseline').mkdir();(inputs/'candidate').mkdir()
   for checkpoint in ['checkpoint','final']:
    shutil.copy2(ROOT/'verification/pressure-references'/reference/(checkpoint+'.json'),inputs/'baseline'/(checkpoint+'.json'))
    shutil.copy2(out/'exports/baseline_logic_unit_999'/(checkpoint+'.json'),inputs/'candidate'/(checkpoint+'.json'))
   many.M=inputs;many.OUT=w/'proof';many.OUT.mkdir();many.TOP='baseline_logic_unit_999';many.main();result=json.loads((many.OUT/'summary.json').read_text());assert result['status']=='PASS';relation_audit.audit(many,w/'audit');report['pressure'][reference]='PASS_ALL_STATE_INIT_EVENT_OUTPUTS';save()
  allcases={}
  for collection in ['new-independent','structural-variants']:
   plan=json.loads((ROOT/'evidence'/collection/'plan.json').read_text())
   for name,c in plan['cases'].items():allcases[name]=(collection,c)
  (out/'rtl').mkdir()
  for name,(collection,c) in allcases.items():
   w=out/'rtl'/name;w.mkdir();source=ROOT/'evidence'/collection/'results'/name
   shutil.copy2(source/'input.v',w/'input.v');shutil.copy2(source/'primitive_decode.v',w/'primitive_decode.v');assert sha(w/'input.v')==c['input_sha256']
   execute(ROOT/'yosys.exe',w,f"read_verilog -icells input.v\nsynth_intel -family cycloneiv -top {c['top']} -run begin:map_cells\nwrite_rtlil candidate-checkpoint.il\n",'export')
   text=execute(tool,w,(source/'rtl-proof.ys').read_text(),'rtl-proof');assert 'Induction step proven: SUCCESS!'in text or 'SAT proof finished - no model found: SUCCESS!'in text
   if(source/'metamorphic-gold.v').exists():
    shutil.copy2(source/'metamorphic-gold.v',w/'metamorphic-gold.v');t=execute(tool,w,(source/'metamorphic-proof.ys').read_text(),'metamorphic-proof');assert 'Induction step proven: SUCCESS!'in t or 'SAT proof finished - no model found: SUCCESS!'in t
   report['rtl'][name]='RTL_INDUCTION_PASS';save();print(name,'RTL_PASS',flush=True)
  report['faults']={}
  for name in ['public_output','explicit_initial']:
   source=ROOT/'evidence/rtl-proof-fault-audit'/name;w=out/('fault-'+name);shutil.copytree(source,w);execute(tool,w,(source/'run.ys').read_text(),'repeat-fault',True);report['faults'][name]='REJECTED'
  report['status']='PASS_REEXPORTED_GRAPHS_FRESH_STATE_RELATIONS_BOTH_PRESSURE_REFERENCES_AND_36_RTL_CASES'
 finally:save()
 print(report['status'],flush=True)
if __name__=='__main__':main()
