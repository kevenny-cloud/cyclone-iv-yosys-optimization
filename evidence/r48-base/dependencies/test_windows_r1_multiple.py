from pathlib import Path
import json,subprocess,os,re
R=Path.cwd();O=R/'execution-20261030/r1-new-boundaries';O.mkdir(exist_ok=True);N=R/'native/windows-r1-xor-complete/yosys.exe';Y=R.parents[2]/'oss-cad-suite/bin/yosys.exe';env=dict(os.environ);env['PATH']=str(Y.parent)+os.pathsep+str(Y.parent.parent/'lib')+os.pathsep+env['PATH'];plan=json.loads((R/'execution-20261030/r1-boundary-plan/plan.json').read_text());out={}
for name,c in plan['cases'].items():
 w=O/name;w.mkdir(exist_ok=True);(w/'input.v').write_bytes((R/'execution-20261030/r1-boundary-plan'/(name+'.v')).read_bytes());(w/'export.ys').write_text('read_verilog input.v\nhierarchy -top t\nproc\nopt_clean\nwrite_rtlil gold.il\npmux_xor_factor\nwrite_rtlil gate.il\nwrite_json gate.json\n')
 with(w/'export.log').open('wb')as f:subprocess.run([str(N),'-Q','-s','export.ys'],cwd=w,stdout=f,stderr=subprocess.STDOUT,check=True)
 match=int(re.search(r'PMUX_XOR_FACTOR: matched=(\d+)',(w/'export.log').read_text(errors='replace'))[1]);assert match==c['expected_matches'],(name,match)
 proof='read_rtlil gold.il\nrename t gold\nread_rtlil gate.il\nrename t gate\nequiv_make gold gate equiv\nhierarchy -top equiv\nequiv_simple\nequiv_status -assert\n';(w/'proof.ys').write_text(proof)
 with(w/'proof.log').open('wb')as f:subprocess.run([str(Y),'-Q','-s','proof.ys'],cwd=w,env=env,stdout=f,stderr=subprocess.STDOUT,check=True)
 out[name]={'matches':match,'equivalence':'PASS'};print(name,match,'PASS',flush=True)
 if name=='two_independent':
  d=json.loads((w/'gate.json').read_text());cells=d['modules']['t']['cells'];xor=next(v for v in cells.values() if v['type']=='$xor');xor['connections']['B']=['0']*len(xor['connections']['B']);(w/'fault.json').write_text(json.dumps(d));(w/'fault.ys').write_text('read_json fault.json\nwrite_rtlil fault.il\n');subprocess.run([str(Y),'-Q','-s','fault.ys'],cwd=w,env=env,stdout=subprocess.DEVNULL,check=True)
  (w/'fault-proof.ys').write_text(proof.replace('gate.il','fault.il'))
  with(w/'fault-proof.log').open('wb')as f:p=subprocess.run([str(Y),'-Q','-s','fault-proof.ys'],cwd=w,env=env,stdout=f,stderr=subprocess.STDOUT)
  assert p.returncode!=0 and 'unproven' in (w/'fault-proof.log').read_text(errors='replace');out[name]['deliberate_fault_rejected']=True
(O/'summary.json').write_text(json.dumps(out,indent=2))
