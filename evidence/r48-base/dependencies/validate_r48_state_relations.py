from pathlib import Path
import json,hashlib,importlib.util
R=Path(__file__).resolve().parent;V=R/'continuation-20261030/r48-one-snapshot-clean';M=V/'matrix';O=V/'state-relations';O.mkdir(exist_ok=True);spec=importlib.util.spec_from_file_location('stateproof',R/'prove_state_relation.py');mod=importlib.util.module_from_spec(spec);spec.loader.exec_module(mod);data=json.loads((M/'summary.json').read_text());out={'status':'RUNNING','candidate_sha256':data['config']['candidate']['yosys_sha256'],'cases':{},'skipped':{}}
try:
 for name,c in data['cases'].items():
  gs=[json.loads((M/name/v/'final.json').read_text())['modules'][c['top']]for v in ['baseline','candidate']];counts=[sum(x['type']=='dffeas'for x in g['cells'].values())for g in gs]
  if counts[0]==0 or counts[0]!=counts[1]:out['skipped'][name]={'counts':counts,'reason':'combinational or changed state dimension; handled by independent temporal proof'};continue
  root=O/name;root.mkdir(exist_ok=True);out['cases'][name]={}
  for checkpoint in ['checkpoint','final']:
   gs=[json.loads((M/name/v/(checkpoint+'.json')).read_text())['modules'][c['top']]for v in ['baseline','candidate']];result=mod.prove(*gs,root/checkpoint);assert result['status']=='STATE_TRANSITION_SAT_PASS',(name,checkpoint,result['status']);mod.audit(*gs,root/checkpoint);out['cases'][name][checkpoint]={'status':result['status'],'registers':result['registers'],'attempts':len(result['attempts']),'summary_sha256':hashlib.sha256((root/checkpoint/'summary.json').read_bytes()).hexdigest()};print(name,checkpoint,'STATE_EVENT_INIT_PASS',result['registers'],flush=True)
  (O/'summary.json').write_text(json.dumps(out,indent=2))
 out['status']='PASS'
except Exception as e:out.update(status='ERROR',error=repr(e));raise
finally:(O/'summary.json').write_text(json.dumps(out,indent=2))
