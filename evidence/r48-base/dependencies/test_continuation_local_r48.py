from pathlib import Path
import subprocess,json,random,os,hashlib
R=Path(__file__).resolve().parent;O=R/'continuation-20261030/r48-one-snapshot-clean/local-sat';O.mkdir(exist_ok=True);N=R/'native/windows-r48-one-snapshot-clean/yosys.exe';Y=R.parents[2]/'oss-cad-suite/bin/yosys.exe';env=dict(os.environ);env['PATH']=str(Y.parent)+os.pathsep+str(Y.parent.parent/'lib')+os.pathsep+env['PATH'];out={}
for seed in range(40):
 rnd=random.Random(5071+seed);w=O/f'case{seed:02d}';w.mkdir(exist_ok=True);width=2+seed%3;fan=2+seed%5;mask=rnd.randrange(1<<(1<<width));flags='(* keep *)'if seed in [3,8,13]else'';yp='output tap,'if seed in [4,9,14]else'';src=f'module t(input[5:0] a,{yp}output[{fan-1}:0] y);wire u,v;\n'
 src+=f'{flags} \\$lut #(.WIDTH({width}),.LUT({1<<width}\'h{mask:x})) root(.A(a[{width-1}:0]),.Y(u));\n'
 inv=mask^((1<<(1<<width))-1);src+=f'\\$lut #(.WIDTH({width}),.LUT({1<<width}\'h{inv:x})) opposite(.A(a[{width-1}:0]),.Y(v));\n'
 for j in range(fan):
  pins=['v'if j%2 else'u']+[f'a[{k}]'for k in range(width-1)]
  if seed%7==0:pins[-1]='a[5]'
  cm=rnd.randrange(1<<(1<<len(pins)));attr='(* keep *)'if seed in [5,10,15]and j==0 else''
  src+=f'{attr} \\$lut #(.WIDTH({len(pins)}),.LUT({1<<len(pins)}\'h{cm:x})) c{j}(.A({{{",".join(reversed(pins))}}}),.Y(y[{j}]));\n'
 if yp:src+='assign tap=u;\n'
 src+='endmodule\n';(w/'input.v').write_text(src,encoding='utf-8');(w/'run.ys').write_text('read_verilog -icells input.v\nhierarchy -top t\nwrite_rtlil gold.il\nwrite_json gold.json\nlut_window\nwrite_rtlil gate.il\nwrite_json gate.json\n',encoding='utf-8')
 with(w/'run.log').open('wb')as f:subprocess.run([str(N),'-Q','-s','run.ys'],cwd=w,stdout=f,stderr=subprocess.STDOUT,check=True)
 proof='read_rtlil gold.il\nrename t gold\nread_rtlil gate.il\nrename t gate\nmiter -equiv -flatten gold gate miter\nprep -top miter\nsat -verify -prove trigger 0 -set-def-inputs miter\n';(w/'proof.ys').write_text(proof)
 with(w/'proof.log').open('wb')as f:subprocess.run([str(Y),'-Q','-s','proof.ys'],cwd=w,env=env,stdout=f,stderr=subprocess.STDOUT,check=True,timeout=45)
 old=json.loads((w/'gold.json').read_text())['modules']['t']['cells'];new=json.loads((w/'gate.json').read_text())['modules']['t']['cells'];assert len(new)<=len(old)
 for name,c in old.items():
  if c.get('attributes',{}).get('keep','').endswith('1'):assert c==new[name]
 out[f'case{seed:02d}']={'status':'PASS','before':len(old),'after':len(new),'kept_cells_unchanged':True};print(seed,len(old),len(new),flush=True)
w=O/'case00';d=json.loads((w/'gate.json').read_text());m=d['modules']['t'];outputs=set(m['ports']['y']['bits']);c=next(c for c in m['cells'].values()if c['type']=='$lut'and c['connections']['Y'][0]in outputs);c['parameters']['LUT']=''.join('1'if x=='0'else'0'for x in c['parameters']['LUT']);(w/'fault.json').write_text(json.dumps(d),encoding='utf-8');(w/'fault.ys').write_text(proof.replace('read_rtlil gate.il','read_json fault.json'))
with(w/'fault.log').open('wb')as f:rc=subprocess.run([str(Y),'-Q','-s','fault.ys'],cwd=w,env=env,stdout=f,stderr=subprocess.STDOUT).returncode
assert rc!=0 and 'proof did fail'in(w/'fault.log').read_text(errors='replace');out['case00']['deliberate_fault_rejected']=True
(O/'summary.json').write_text(json.dumps({'status':'PASS','candidate_sha256':hashlib.sha256(N.read_bytes()).hexdigest(),'proof_tool_sha256':hashlib.sha256(Y.read_bytes()).hexdigest(),'cases':out},indent=2))
