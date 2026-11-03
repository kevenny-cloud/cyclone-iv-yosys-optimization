from pathlib import Path
import json,subprocess,hashlib,os
R=Path(__file__).resolve().parent;V=R/'continuation-20261030/r48-one-snapshot-clean';O=V/'strong-rule-miters';O.mkdir(exist_ok=True);Y=R.parents[2]/'oss-cad-suite/bin/yosys.exe';env=dict(os.environ);env['PATH']=str(Y.parent)+os.pathsep+str(Y.parent.parent/'lib')+os.pathsep+env['PATH'];types={'$dff','$dffe','$adff','$adffe','$aldff','$aldffe','$dlatch','$adlatch','$sdff','$sdffe','$sdffce','$ff'};out={}
def run(w,s,stem):
 (w/(stem+'.ys')).write_text(s,encoding='utf-8')
 with(w/(stem+'.log')).open('wb')as f:p=subprocess.run([str(Y),'-Q','-s',stem+'.ys'],cwd=w,env=env,stdout=f,stderr=subprocess.STDOUT,timeout=45)
 return p.returncode
for group in ['xor','xor-multiple','arithmetic']:
 for src in sorted((V/'boundaries'/group).iterdir()):
  if not src.is_dir()or not(src/'gold.il').exists():continue
  name=group+'-'+src.name;w=O/name;w.mkdir(exist_ok=True);states=[];graphs=[]
  for phase in ['gold','gate']:
   (w/(phase+'.il')).write_bytes((src/(phase+'.il')).read_bytes());assert run(w,f'read_rtlil {phase}.il\nwrite_json {phase}.json\n',phase+'-json')==0
   d=json.loads((w/(phase+'.json')).read_text(encoding='utf-8'));m=d['modules']['t'];state={};init={}
   for n,net in m['netnames'].items():
    if 'init'in net.get('attributes',{}):
     bits=net['bits'];vals=list(reversed(net['attributes']['init']));init.update(zip(bits,vals))
   for n,c in list(m['cells'].items()):
    if c['type']not in types:continue
    state[n]={'type':c['type'],'params':c['parameters'],'output_init':{p:[init.get(b,'x')for b in bits]for p,bits in c['connections'].items()if c['port_directions'][p]=='output'}}
    stem='ff_'+hashlib.sha256(n.encode()).hexdigest()[:16]
    for p,bits in c['connections'].items():
     pn=stem+'_'+p;direction='input'if c['port_directions'][p]=='output'else'output';m['ports'][pn]={'direction':direction,'bits':bits};m['netnames'][pn]={'hide_name':0,'bits':bits,'attributes':{}}
    del m['cells'][n]
   for net in m['netnames'].values():net.get('attributes',{}).pop('init',None)
   states.append(state);graphs.append(d);(w/(phase+'-cut.json')).write_text(json.dumps(d,ensure_ascii=False),encoding='utf-8')
  assert states[0]==states[1],name
  proof='read_json gold-cut.json\nrename t gold\nread_json gate-cut.json\nrename t gate\nmiter -equiv -flatten gold gate miter\nprep -top miter\nsat -verify -prove trigger 0 -set-def-inputs -show-inputs -show-outputs miter\n'
  rc=run(w,proof,'proof');assert rc==0 and 'SAT proof finished - no model found: SUCCESS!'in(w/'proof.log').read_text(errors='replace'),name
  out[name]={'status':'COMBINATIONAL_MITER_ALL_PUBLIC_AND_STATE_PORTS_PASS','state_elements':len(states[0]),'init_and_state_type_parameters_preserved':True}
  if name=='xor-decode_default':
   fault=json.loads(json.dumps(graphs[1]));c=next(c for c in fault['modules']['t']['cells'].values()if c['type']=='$xor');c['connections']['B'][0]='0';(w/'fault-cut.json').write_text(json.dumps(fault),encoding='utf-8');rc=run(w,proof.replace('gate-cut.json','fault-cut.json'),'fault');text=(w/'fault.log').read_text(errors='replace');assert rc!=0 and ('proof did fail'in text or 'SAT proof finished - model found: FAIL!'in text);out[name]['deliberate_fault_rejected']=True
  print(name,'PASS',flush=True)
(O/'summary.json').write_text(json.dumps({'status':'PASS','candidate_sha256':hashlib.sha256((V/'program/yosys.exe').read_bytes()).hexdigest(),'proof_tool_sha256':hashlib.sha256(Y.read_bytes()).hexdigest(),'tests':out},indent=2),encoding='utf-8')
