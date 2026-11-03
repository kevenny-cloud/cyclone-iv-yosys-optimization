from pathlib import Path
import json,subprocess,collections,shutil,sys,hashlib
R=Path(__file__).resolve().parent;label=sys.argv[1];V=R/'validation-r10-20261030';O=R/'continuation-20261030'/label;M=O/'matrix';M.mkdir(exist_ok=True);old=json.loads((V/'matrix/summary.json').read_text());exe=R/'native'/('windows-'+label)/'yosys.exe';sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();report={'status':'EXPORTING','scope':'Frozen 78 cases, unchanged original reference','config':dict(old['config']),'cases':{}};report['config']['candidate']={'yosys':str(exe),'yosys_sha256':sha(exe),'abc_sha256':sha(exe.parent/'yosys-abc.exe')};assert report['config']['candidate']['abc_sha256']==old['config']['baseline']['abc_sha256'];resource={}
for name,c in old['cases'].items():
 src=V/'matrix'/name;w=M/name;w.mkdir(exist_ok=True);shutil.copytree(src/'baseline',w/'baseline',dirs_exist_ok=True);shutil.copytree(src/'candidate',w/'candidate',ignore=shutil.ignore_patterns('*.json','*.il','*.log'),dirs_exist_ok=True)
 for rel,h in c['input_sha256'].items():
  assert sha(src/rel)==h;dst=w/rel;dst.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(src/rel,dst)
 for f,h in c['variants']['baseline']['sha256'].items():assert sha(w/'baseline'/f)==h
 with(w/'candidate/run.log').open('wb')as f:subprocess.run([str(exe),'-Q','-s','run.ys'],cwd=w/'candidate',stdout=f,stderr=subprocess.STDOUT,check=True,timeout=120)
 text=(w/'candidate/run.log').read_text(encoding='utf-8',errors='replace');assert not any(x in text for x in ['ABC: Error:','ABC: ERROR:','Assertion failed'])
 entry=json.loads(json.dumps(c));entry['variants']['candidate']={'status':'EXPORTED','abc_errors':[],'sha256':{f:sha(w/'candidate'/f)for f in ['checkpoint.il','checkpoint.json','final.il','final.json','run.ys','run.log']}};report['cases'][name]=entry
 row={}
 for v in ['baseline','candidate']:
  cells=json.loads((w/v/'final.json').read_text())['modules'][c['top']]['cells'];h=collections.Counter(x['type']for x in cells.values());row[v]={'logic':h['cycloneiv_lcell_comb']+h['$not'],'dff':h['dffeas']}
 resource[name]=row;print(name,row['baseline']['logic'],row['candidate']['logic'],flush=True)
 (M/'summary.json').write_text(json.dumps(report,indent=2));(O/'resources.json').write_text(json.dumps({'cases':resource,'increases':[[n,v['baseline']['logic'],v['candidate']['logic']]for n,v in resource.items()if v['candidate']['logic']>v['baseline']['logic']]},indent=2))
report['status']='EXPORTED';(M/'summary.json').write_text(json.dumps(report,indent=2));print('EXPORTED_ALL78',flush=True)
