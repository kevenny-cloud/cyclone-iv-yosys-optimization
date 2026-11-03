from pathlib import Path
import json,os,subprocess,hashlib
import prove_state_relation as proof
R=Path(__file__).resolve().parent;V=R/'continuation-20261030/r48-one-snapshot-clean';D=R/'continuation-20261030/next-plan-execution';M=D/'r50-matrix';O=D/'r50-matrix-proofs';assert not O.exists();O.mkdir()
meta=json.loads((M/'summary.json').read_text());report={'status':'RUNNING','cases':{}};env=dict(os.environ);env['PATH']=str(proof.YOSYS.parent)+os.pathsep+str(proof.YOSYS.parent.parent/'lib')+os.pathsep+env['PATH']
try:
 for name,c in meta['cases'].items():
  (O/name).mkdir();report['cases'][name]={}
  for cp in ['checkpoint','final']:
   a,b=[json.loads(p.read_text())['modules'][c['top']]for p in [V/'matrix'/name/'candidate'/(cp+'.json'),M/name/(cp+'.json')]];w=O/name/cp
   if proof.registers(a):
    result=proof.prove(a,b,w);assert result['status']=='STATE_TRANSITION_SAT_PASS';proof.audit(a,b,w);status=result['status']
   else:
    w.mkdir();ga,n=proof.emit(a,'gold',{});gb,m=proof.emit(b,'gate',{});assert n==m
    (w/'relation.v').write_text(ga+gb);(w/'run.ys').write_text('read_verilog relation.v\nmiter -equiv -flatten gold gate miter\nprep -top miter\nsat -verify -prove trigger 0 -set-def-inputs miter\n')
    with(w/'run.log').open('wb')as f:subprocess.run([str(proof.YOSYS),'-Q','-s','run.ys'],cwd=w,env=env,stdout=f,stderr=subprocess.STDOUT,check=True,timeout=120)
    assert 'SAT proof finished - no model found: SUCCESS!'in(w/'run.log').read_text();status='COMBINATIONAL_SAT_PASS'
   report['cases'][name][cp]=status;print(name,cp,status,flush=True)
   (O/'summary.json').write_text(json.dumps(report,indent=2))
 report['status']='PASS_R48_TO_R50_78_DESIGNS_156_CHECKPOINTS'
finally:(O/'summary.json').write_text(json.dumps(report,indent=2))
