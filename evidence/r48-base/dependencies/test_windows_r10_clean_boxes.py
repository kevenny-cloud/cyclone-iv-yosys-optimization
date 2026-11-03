from pathlib import Path
import subprocess,json,os,hashlib
R=Path.cwd();O=R/'execution-20261030/r10-clean-box-tests';O.mkdir(exist_ok=True);N=R/'native/windows-r10-clean/yosys.exe';Y=R.parents[2]/'oss-cad-suite/bin/yosys.exe';env=dict(os.environ);env['PATH']=str(Y.parent)+os.pathsep+str(Y.parent.parent/'lib')+os.pathsep+env['PATH'];head='''(* abc9_box, blackbox *) module box1(input i,output o);specify(i=>o)=1;endspecify endmodule
''';cases={'one_box':head+'module top(input[3:0] a,output y);wire z;box1 u(.i(a[0]^a[1]),.o(z));assign y=z^(a[2]&a[3]);endmodule\n','two_boxes':head+'module top(input[5:0] a,output [1:0] y);wire z,t;box1 u(.i(a[0]^a[1]),.o(z));box1 v(.i(a[2]&z),.o(t));assign y={t^(a[3]&a[4]),z^a[5]};endmodule\n','dangling_box':head+'module top(input[3:0] a,output y);wire z;box1 u(.i(a[0]),.o(z));assign y=a[1]^(a[2]&a[3]);endmodule\n'};out={}
for name,source in cases.items():
 w=O/name;w.mkdir(exist_ok=True);(w/'input.v').write_text(source);(w/'run.ys').write_text('read_verilog -specify input.v\nhierarchy -top top\nproc\ntechmap\nopt\nwrite_json gold.json\nabc9 -lut 4\ncheck -assert\nwrite_json gate.json\n')
 with(w/'run.log').open('wb')as f:p=subprocess.run([str(N),'-Q','-s','run.ys'],cwd=w,stdout=f,stderr=subprocess.STDOUT,timeout=20)
 assert p.returncode==0,(name,p.returncode)
 identities=[]
 for phase in ['gold','gate']:
  d=json.loads((w/(phase+'.json')).read_text(encoding='utf-8'));m=d['modules']['top'];ids={}
  for nn,c in list(m['cells'].items()):
   if c['type'] not in d['modules'] or not int(d['modules'][c['type']]['attributes'].get('blackbox','0'),2):continue
   ids[nn]={'type':c['type'],'parameters':c['parameters']}
   for port,bits in c['connections'].items():
    pn='box_'+hashlib.sha256(nn.encode()).hexdigest()[:12]+'_'+port;direction='input' if c['port_directions'][port]=='output' else 'output';m['ports'][pn]={'direction':direction,'bits':bits};m['netnames'][pn]={'hide_name':0,'bits':bits,'attributes':{}}
   del m['cells'][nn]
  identities.append(ids);d['modules']={'top':m};(w/(phase+'-cut.json')).write_text(json.dumps(d,ensure_ascii=False),encoding='utf-8')
 # A genuinely unused box may be swept before the packing snapshot.
 if name!='dangling_box':assert identities[0]==identities[1]
 (w/'proof.ys').write_text('read_json gold-cut.json\nrename top gold\nread_json gate-cut.json\nrename top gate\nequiv_make gold gate equiv\nhierarchy -top equiv\nequiv_simple\nequiv_status -assert\n')
 with(w/'proof.log').open('wb')as f:p=subprocess.run([str(Y),'-Q','-s','proof.ys'],cwd=w,env=env,stdout=f,stderr=subprocess.STDOUT,timeout=30)
 assert p.returncode==0,name;out[name]={'equivalence_of_all_box_inputs_and_public_outputs':'PASS','boxes_before':len(identities[0]),'boxes_after':len(identities[1])};print(name,'PASS',flush=True)
(O/'summary.json').write_text(json.dumps(out,indent=2))
