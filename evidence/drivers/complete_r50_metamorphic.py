"""Frozen independent development/holdout/metamorphic set and RTL proof entry."""
from pathlib import Path
import json,hashlib,random,shutil,subprocess,os,sys
R=Path(__file__).resolve().parent;D=R/'continuation-20261030/next-plan-execution'
O=D/'metamorphic-structure-v1';sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
def freeze():
 assert not O.exists();O.mkdir()
 plan={'status':'FROZEN_BEFORE_TESTING','seed':718339,'development':[],'holdout':[],'metamorphic':{},'cases':{},'candidate_algorithm_frozen_before_holdout':True}
 for i in range(24):
  rnd=random.Random(718339+i);width=[1,3,7,13][i%4];name=f'generic_{i:02d}';w=O/name;w.mkdir();top='unit'
  mask=rnd.randrange(1<<width);seq=i%2==1
  ports=f'input clk,rst,en,input[2:0] sel,input[{width-1}:0] a,b,c,d,output'+(' reg'if seq else'')+f'[{width-1}:0] y,output[{width-1}:0] tap'
  rtl=f'module {top}({ports});\nreg[{width-1}:0] u,v;\n'
  rtl+=f"always @* case(sel) 0:u=a;1:u=b;2:u=c;3:u=d;4:u=a^{width}'d{mask};default:u=b^d;endcase\n"
  rtl+=f"always @* case(sel[1:0]) 0:v=u^c;1:v=a&d;2:v=u|b;default:v=c^{width}'d{mask};endcase\n"
  rtl+='assign tap='+('u;'if i%3==0 else'v^u;')+'\n'
  if seq:rtl+=f"initial y={width}'d{mask};always @(posedge clk)if(rst)y<={width}'d{mask};else if(en)y<=v;\n"
  else:rtl+='assign y=v;\n'
  rtl+='endmodule\n';(w/'input.v').write_text(rtl,encoding='utf-8')
  plan['cases'][name]={'top':top,'width':width,'explicit_state':seq,'input_sha256':sha(w/'input.v'),'set':'development'if i<12 else'holdout'};plan['development'if i<12 else'holdout'].append(name)
  if i in [0,5,12,17]:
   other=name+'_renamed';q=O/other;q.mkdir();text=rtl.replace('module unit(','module renamed_unit(').replace(' u,',' internal_u,').replace(' u=',' internal_u=').replace(':u=',':internal_u=').replace('=u','=internal_u').replace(';1:u',';1:internal_u').replace(';2:u',';2:internal_u').replace(';3:u',';3:internal_u').replace(';4:u',';4:internal_u').replace(':u=',':internal_u=')
   # Token-wise rename prevents accidental changes to syntax or constants.
   import re
   text=re.sub(r'\bu\b','internal_u',rtl).replace('module unit(','module renamed_unit(')
   (q/'input.v').write_text(text,encoding='utf-8');plan['cases'][other]={**plan['cases'][name],'top':'renamed_unit','input_sha256':sha(q/'input.v'),'set':'metamorphic'};plan['metamorphic'][other]=name
 (O/'plan.json').write_text(json.dumps(plan,indent=2),encoding='utf-8')
def run(candidate):
 plan=json.loads((O/'plan.json').read_text());out=O/'results';assert not out.exists();out.mkdir();report={'status':'RUNNING','candidate':str(candidate),'candidate_sha256':sha(candidate),'plan_sha256':sha(O/'plan.json'),'cases':{}}
 Y=R.parents[2]/'oss-cad-suite/bin/yosys.exe';env=dict(os.environ);env['PATH']=str(Y.parent)+os.pathsep+str(Y.parent.parent/'lib')+os.pathsep+env['PATH'];model=R/'models/enable2-clean1/primitive_decode_sync.v'
 def execute(exe,w,script,label):
  (w/(label+'.ys')).write_text(script,encoding='utf-8')
  with(w/(label+'.log')).open('wb')as f:subprocess.run([str(exe),'-Q','-s',label+'.ys'],cwd=w,env=env,stdout=f,stderr=subprocess.STDOUT,check=True,timeout=120)
 try:
  for name,c in plan['cases'].items():
   w=out/name;w.mkdir();shutil.copy2(O/name/'input.v',w/'input.v');assert sha(w/'input.v')==c['input_sha256'];top=c['top']
   for label,exe in [('baseline',R/'native/windows-reference/yosys.exe'),('candidate',candidate)]:
    s=f'read_verilog -icells input.v\nsynth_intel -family cycloneiv -top {top} -run begin:map_cells\nwrite_rtlil {label}-checkpoint.il\nwrite_json {label}-checkpoint.json\ntechmap -map +/intel/cycloneiv/cells_map.v\nclean -purge\nwrite_json {label}-final.json\n'
    execute(exe,w,s,label)
   gs=[json.loads((w/(v+'-final.json')).read_text())['modules'][top]for v in ['baseline','candidate']]
   count=lambda g:sum(x['type']in ['cycloneiv_lcell_comb','$not']for x in g['cells'].values())
   areas=list(map(count,gs));assert areas[1]<=areas[0],(name,'resource increase',areas)
   shutil.copy2(model,w/'primitive_decode.v')
   # RTL has explicit initial state for every register in this test collection.
   s=f'read_verilog input.v\nhierarchy -top {top}\nproc\nflatten\nmemory_map\nopt\nclk2fflogic\nrename {top} gold\nwrite_rtlil gold.il\ndesign -reset\nread_rtlil candidate-checkpoint.il\ntechmap -map primitive_decode.v\nhierarchy -top {top}\nflatten\nopt\nclk2fflogic\nrename {top} gate\nwrite_rtlil gate.il\nread_rtlil gold.il\nmiter -equiv -flatten gold gate miter\nprep -top miter\nsat -verify -tempinduct -seq 1 -maxsteps 24 -prove trigger 0 -set-def-inputs miter\n'
   execute(Y,w,s,'rtl-proof');log=(w/'rtl-proof.log').read_text();assert 'Induction step proven: SUCCESS!'in log or 'SAT proof finished - no model found: SUCCESS!'in log
   report['cases'][name]={'status':'PASS_RTL_TO_CHECKPOINT_INDUCTION','logic':areas,'explicit_state':c['explicit_state'],'proof_sha256':sha(w/'rtl-proof.log')};(out/'summary.json').write_text(json.dumps(report,indent=2));print(name,areas,'RTL_PASS',flush=True)
  for name,base in plan['metamorphic'].items():assert report['cases'][name]['logic'][1]<=report['cases'][name]['logic'][0]
  report['status']='PASS_8_FROZEN_STRUCTURAL_VARIANTS_RTL_INDUCTION_AND_NO_RESOURCE_INCREASE'
 finally:(out/'summary.json').write_text(json.dumps(report,indent=2))
if __name__=='__main__':
 if sys.argv[1]=='freeze':freeze()
 else:run(Path(sys.argv[1]))
