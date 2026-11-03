"""Fresh actual-candidate stress export against byte-verified reference graphs."""
from pathlib import Path
import hashlib,json,shutil,subprocess,sys,collections
R=Path(__file__).resolve().parent
label=sys.argv[1];V=R/'continuation-20261030'/label;O=V/'stress';O.mkdir(exist_ok=False)
read=lambda p:json.loads(p.read_text(encoding='utf-8-sig'))
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
P=R/'continuation-20261030/r47-user-stress';old=read(P/'summary.json');top='baseline_logic_unit_999'
case=json.loads(json.dumps(old['cases'][top]));W=O/top;W.mkdir()
cfg=read(V/'timing-config.json');assert sha(V/'program/yosys.exe')==cfg['candidate']['yosys_sha256']
for n,h in case['input_sha256'].items():
 p=P/top/n;assert sha(p)==h;shutil.copy2(p,W/n)
for n,h in case['variants']['baseline']['sha256'].items():assert sha(P/top/'baseline'/n)==h
shutil.copytree(P/top/'baseline',W/'baseline')
F=W/'candidate';F.mkdir()
for n in case['input_sha256']:shutil.copy2(W/n,F/n)
(F/'run.ys').write_bytes((W/'baseline/run.ys').read_bytes())
with(F/'run.log').open('wb')as out:subprocess.run([str(V/'program/yosys.exe'),'-Q','-s','run.ys'],cwd=F,stdout=out,stderr=subprocess.STDOUT,check=True,timeout=600)
log=(F/'run.log').read_text(encoding='utf-8',errors='replace');assert 'ABC: Error:'not in log and 'ABC: ERROR:'not in log
h=collections.Counter(c['type']for c in read(F/'final.json')['modules'][top]['cells'].values())
case['variants']['candidate']={'sha256':{n:sha(F/n)for n in ['run.ys','run.log','checkpoint.il','checkpoint.json','final.il','final.json']},'logic':h['cycloneiv_lcell_comb']+h['$not'],'dff':h['dffeas'],'by_type':dict(h)}
report={'status':'EXPORTED','config':cfg,'cases':{top:case},'provenance':{'original_reference':str(P/'summary.json'),'original_reference_summary_sha256':sha(P/'summary.json')}}
(O/'summary.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
hist=O/'historical';hist.mkdir();Z=hist/top;Z.mkdir()
for n in case['input_sha256']:shutil.copy2(W/n,Z/n)
oldhist=read(R.parents[2]/'p47hist/summary.json');prior=oldhist['cases']['s']['variants']['baseline']
for n,h in prior['sha256'].items():assert sha(R.parents[2]/'p47hist/s/baseline'/n)==h
shutil.copytree(R.parents[2]/'p47hist/s/baseline',Z/'baseline');shutil.copytree(F,Z/'candidate')
hc=json.loads(json.dumps(case));hc['variants']['baseline']=prior
(hist/'summary.json').write_text(json.dumps({'status':'EXPORTED','config':{'baseline':oldhist['config']['baseline'],'candidate':cfg['candidate']},'cases':{top:hc}},indent=2),encoding='utf-8')
resources={}
for name,c in [('current',case),('historical',hc)]:
 b,g=[c['variants'][v]['logic']for v in ['baseline','candidate']]
 resources[name]={'baseline':b,'candidate':g,'reduction_percent':100*(b-g)/b,'strict20_pass':(b-g)*5>b,'dff_nonincrease':c['variants']['candidate']['dff']<=c['variants']['baseline']['dff']}
 assert resources[name]['strict20_pass']and resources[name]['dff_nonincrease']
(O/'resources.json').write_text(json.dumps(resources,indent=2),encoding='utf-8')
# Combined ten-case fixture pointer for a single declared rules132 batch.
T=V/'ten-case-fixtures';T.mkdir();combined=read(V/'matrix/summary.json')
for n,c in combined['cases'].copy().items():
 if n not in ['test1','test2','test3','test4','pmux_case01_balanced','pmux_case02_pipeline','pmux_case03_crossbar','pmux_case04_feedback','pmux_case05_nested']:del combined['cases'][n];continue
 d=T/n;d.mkdir()
 for f in c['input_sha256']:shutil.copy2(V/'matrix'/n/f,d/f)
combined['cases'][top]=case;d=T/top;d.mkdir()
for n in case['input_sha256']:shutil.copy2(W/n,d/n)
(T/'summary.json').write_text(json.dumps(combined,indent=2),encoding='utf-8')
(V/'ten-case-pointer.json').write_text(json.dumps({'results':str(T)},indent=2),encoding='utf-8')
print('STRESS_EXPORT_AND_TEN_CASE_FIXTURES_READY',resources)
