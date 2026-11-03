from pathlib import Path
import subprocess,json,os,re
R=Path.cwd();O=R/'execution-20261030/r3-boundaries';O.mkdir(exist_ok=True);N=R/'native/windows-r3-arith-share/yosys.exe';Y=R.parents[2]/'oss-cad-suite/bin/yosys.exe';env=dict(os.environ);env['PATH']=str(Y.parent)+os.pathsep+str(Y.parent.parent/'lib')+os.pathsep+env['PATH'];out={}
for width in [1,4,9]:
 for op in ['+','-']:
  for keep in [False,True]:
   name=f'w{width}'+('add' if op=='+' else 'sub')+('-keep' if keep else '');w=O/name;w.mkdir(exist_ok=True);s=f'module t(input [1:0] sel,input [{width-1}:0] a,b,c,d,e,f,g,h,'+('(* keep *) ' if keep else '')+f'output reg [{width-1}:0] y);always @* case(sel) 0:y=a{op}b;1:y=c{op}d;2:y=e{op}f;default:y=g{op}h;endcase endmodule\n';(w/'input.v').write_text(s)
  # Recreate an explicit, independently verified branch operation.
   (w/'export.ys').write_text('read_verilog input.v\nhierarchy -top t\nproc\nopt_clean\nwrite_rtlil gold.il\npmux_xor_factor\nwrite_rtlil gate.il\n')
   with(w/'export.log').open('wb')as f:subprocess.run([str(N),'-Q','-s','export.ys'],cwd=w,stdout=f,stderr=subprocess.STDOUT,check=True)
   matched=int(re.search(r'PMUX_XOR_FACTOR: matched=(\d+)',(w/'export.log').read_text(errors='replace'))[1]);assert matched==(0 if keep else 1),(name,matched)
   (w/'proof.ys').write_text('read_rtlil gold.il\nrename t gold\nread_rtlil gate.il\nrename t gate\nequiv_make gold gate equiv\nhierarchy -top equiv\nequiv_simple\nequiv_status -assert\n')
   with(w/'proof.log').open('wb')as f:subprocess.run([str(Y),'-Q','-s','proof.ys'],cwd=w,env=env,stdout=f,stderr=subprocess.STDOUT,check=True,timeout=60)
   out[name]={'matched':matched,'equivalence':'PASS'};print(name,'PASS',flush=True)
(O/'summary.json').write_text(json.dumps(out,indent=2))
