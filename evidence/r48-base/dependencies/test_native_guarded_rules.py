"""Independent SAT checks for decoded defaults/regions and native LUT cuts."""
from pathlib import Path
import collections, hashlib, json, os, random, subprocess, sys
R=Path(__file__).resolve().parent
V=R/'continuation-20261030'/sys.argv[1];O=V/'native-rules';O.mkdir(exist_ok=True)
N=V/'program/yosys.exe';Y=R.parents[2]/'oss-cad-suite/bin/yosys.exe'
env=dict(os.environ);env['PATH']=str(Y.parent)+os.pathsep+str(Y.parent.parent/'lib')+os.pathsep+env['PATH']
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
cases={}

def table(bits,width,explicit=None,default=None,keep=False,alias=False):
 domain=1<<bits;explicit=list(range(domain-1))if explicit is None else explicit
 default=f'd[{(domain-1)*width} +: {width}]'if default is None else default
 control=f'wire[{bits-1}:0] s={{'+','.join(['sel[0]']*bits)+'};'if alias else f'wire[{bits-1}:0] s=sel;'
 attr='(* keep *) 'if keep else ''
 rows='\n'.join(f"{bits}'d{k}: z=d[{k*width} +: {width}];"for k in explicit)
 return f'module t(input[{bits-1}:0] sel,input[{domain*width-1}:0] d,output[{width-1}:0] y);\n{control}\n{attr}reg[{width-1}:0] z;always @* case(s)\n{rows}\ndefault:z={default};\nendcase\nassign y=z;endmodule\n'

for b in [2,3,4]:
 for w in [1,3,5]:cases[f'default_b{b}_w{w}']=(table(b,w),'decode',False,True)
cases['default_b5_w1']=(table(5,1),'decode',False,True)
for values in [[0,1,4],[0,2,3,5,6],[1,2,4,7]]:
 cases['holes_'+''.join(map(str,values))]=(table(3,3,values),'decode',False,False)
cases['kept_output']=(table(2,3,keep=True),'decode',True,False)
cases['aliased_control']=(table(2,3,alias=True),'decode',True,False)
cases['unknown_default']=(table(2,3,default="3'bxxx"),'decode',True,False)
src=table(2,3).replace("2'd1: z=d[3 +: 3];","2'd1: z=3'bxxx;")
cases['unknown_explicit_row']=(src,'decode',True,False)

def selector(name,control,rows,width=3):
 return f'reg[{width-1}:0] {name};always @* case({control})\n'+''.join(f"2'd{k}:{name}={value};\n"for k,value in enumerate(rows[:3]))+f'default:{name}={rows[3]};endcase\n'
head='module t(input[1:0] s,t,r,input[35:0] d,output[5:0] y);\n'
u=selector('u','s',[f'd[{j*3} +: 3]'for j in range(4)])
v=selector('v','t',['u','d[12 +: 3]','d[15 +: 3]','d[18 +: 3]'])
cases['shared_external']=(head+u+v+'assign y={u,v};endmodule\n','decode',True,False)
w=selector('w','r',['u','d[21 +: 3]','d[24 +: 3]','d[27 +: 3]'])
cases['shared_internal_region']=(head+u+v+w+'assign y={v,w};endmodule\n','decode',True,False)
cases['arbitrary_selects']=(r'''module t(input[2:0] s,input[11:0] d,output[2:0] y);
\$pmux #(.WIDTH(3),.S_WIDTH(3)) p(.A(d[2:0]),.B(d[11:3]),.S(s),.Y(y));endmodule
''','decode',True,False)

for seed in range(16):
 rng=random.Random(9417+seed)
 if seed<8:
  low=sum(((((r>>0)&1)&((r>>2)&1))^((r>>1)&1))<<r for r in range(8))
  middle=sum(((((r>>0)&1)^((r>>1)&1)^((r>>2)&1))&((r>>3)&1))<<r for r in range(16))
  top=rng.randrange(1,255)
  src=f'''module t(input[6:0] a,output y);wire u,v;
  \\$lut #(.WIDTH(3),.LUT(8'h{low:x})) l(.A(a[2:0]),.Y(u));
  \\$lut #(.WIDTH(4),.LUT(16'h{middle:x})) m(.A({{a[5],a[4],a[3],u}}),.Y(v));
  \\$lut #(.WIDTH(3),.LUT(8'h{top:x})) h(.A({{a[1],a[0],v}}),.Y(y));endmodule
  '''
 else:
  low=rng.randrange(1,65535);top=rng.randrange(1,65535)
  src=f'''module t(input[6:0] a,output y);wire u,v;
  \\$lut #(.WIDTH(4),.LUT(16'h{low:x})) l(.A(a[3:0]),.Y(u));
  \\$lut #(.WIDTH(1),.LUT(2'b10)) m(.A(u),.Y(v));
  \\$lut #(.WIDTH(4),.LUT(16'h{top:x})) h(.A({{a[6],a[5],a[4],v}}),.Y(y));endmodule
  '''
 cases[f'cut_{seed:02d}']=(src,'cuts',False,False)

report={'status':'RUNNING','candidate_sha256':sha(N),'proof_tool':str(Y),'proof_tool_sha256':sha(Y),'cases':{}}
try:
 for name,(src,kind,unchanged,positive)in cases.items():
  W=O/name;W.mkdir(exist_ok=True);(W/'input.v').write_text(src,encoding='utf-8')
  command='pmux_decode_tree -partial -vector -leaf 0 -rank 0'if kind=='decode'else'lut_window'
  script='read_verilog -icells input.v\nhierarchy -top t\nproc\nopt_clean\nwrite_rtlil gold.il\nwrite_json gold.json\n'+command+'\nwrite_rtlil gate.il\nwrite_json gate.json\n'
  (W/'run.ys').write_text(script,encoding='utf-8')
  with(W/'run.log').open('wb')as out:subprocess.run([str(N),'-Q','-s','run.ys'],cwd=W,stdout=out,stderr=subprocess.STDOUT,check=True,timeout=60)
  graphs=[json.loads((W/(v+'.json')).read_text())['modules']['t']for v in ['gold','gate']]
  before,after=[len(g['cells'])for g in graphs]
  same=graphs[0]==graphs[1]
  if unchanged:assert same,(name,'guard altered graph')
  if positive:assert not same,(name,'expected decoded normalization was missed')
  if kind=='cuts':assert after<=before,(name,'LUT increase')
  if unchanged:
   method='STRUCTURAL_IDENTITY'
  else:
   proof='read_rtlil gold.il\nrename t gold\nread_rtlil gate.il\nrename t gate\nmiter -equiv -flatten gold gate miter\nprep -top miter\nsat -verify -prove trigger 0 -set-def-inputs miter\n'
   (W/'proof.ys').write_text(proof,encoding='utf-8')
   with(W/'proof.log').open('wb')as out:subprocess.run([str(Y),'-Q','-s','proof.ys'],cwd=W,env=env,stdout=out,stderr=subprocess.STDOUT,check=True,timeout=60)
   assert 'SAT proof finished - no model found: SUCCESS!'in(W/'proof.log').read_text(encoding='utf-8',errors='replace')
   method='INDEPENDENT_COMBINATIONAL_SAT_PASS'
  report['cases'][name]={'status':'PASS','kind':kind,'method':method,'before_cells':before,'after_cells':after,'required_skip':unchanged,'required_match':positive,'sha256':{p.name:sha(p)for p in W.iterdir()if p.is_file()}}
  print(name,method,before,after,flush=True)
 report['status']='PASS'
except Exception as e:
 report.update(status='ERROR',error=repr(e));raise
finally:(O/'summary.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
