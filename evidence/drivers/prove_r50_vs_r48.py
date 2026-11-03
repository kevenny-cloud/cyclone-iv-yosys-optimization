from pathlib import Path
import json
import prove_state_relation as proof
R=Path(__file__).resolve().parent
D=R/'continuation-20261030/next-plan-execution'
M=D/'r50-fullflow-diagnostic-v1';O=D/'r50-state-proofs-v1';assert not O.exists();O.mkdir()
report={'status':'RUNNING','cases':{}}
try:
 for d in M.iterdir():
  if not d.is_dir()or d.name=='test2':continue
  report['cases'][d.name]={};(O/d.name).mkdir()
  for cp in ['checkpoint','final']:
   a,b=[json.loads((d/v/(cp+'.json')).read_text())['modules'][d.name]for v in ['baseline','candidate']]
   out=O/d.name/cp;result=proof.prove(a,b,out);assert result['status']=='STATE_TRANSITION_SAT_PASS';proof.audit(a,b,out)
   report['cases'][d.name][cp]=result['status'];print(d.name,cp,'PASS',flush=True)
   (O/'summary.json').write_text(json.dumps(report,indent=2))
 report['status']='STATEFUL_TEN_FIXTURE_SCOPE_PASS_TEST2_SEPARATE'
finally:(O/'summary.json').write_text(json.dumps(report,indent=2))
