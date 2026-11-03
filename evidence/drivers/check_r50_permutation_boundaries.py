"""Exhaustive four-input functional evaluation at memo/visit limits."""
from pathlib import Path
import json,subprocess,hashlib
R=Path(__file__).resolve().parent;D=R/'continuation-20261030/next-plan-execution'
O=D/'permutation-limits-v1';assert not O.exists();O.mkdir()
Y=D/'r50-regenerated-final/program/yosys.exe'
report={'status':'RUNNING','candidate_sha256':hashlib.sha256(Y.read_bytes()).hexdigest(),'cases':{}}
for name,count,selected in [('cache_limit',530,True),('visit_limit',16390,True),('partial_selection',12,False)]:
 w=O/name;w.mkdir();src=f'module t(input[3:0] a,output[{count-1}:0] y);\n'
 for i in range(count):
  attr='(* keep *)'if i%37==0 else''
  mask=(i*109+0x53a9)&65535
  src+=f'{attr} \\$lut #(.WIDTH(4),.LUT(16\'h{mask:x})) c{i}(.A(a),.Y(y[{i}]));\n'
 src+='endmodule\n';(w/'input.v').write_text(src)
 s='read_verilog -icells input.v\nhierarchy -top t\nwrite_json gold.json\n'+(''if selected else'select t/c0\n')+'lut_window -work-limit 1\nselect *\nwrite_json gate.json\n';(w/'run.ys').write_text(s)
 with(w/'run.log').open('wb')as f:subprocess.run([str(Y),'-Q','-s','run.ys'],cwd=w,stdout=f,stderr=subprocess.STDOUT,check=True,timeout=120)
 a,b=[json.loads((w/(v+'.json')).read_text())['modules']['t']for v in ['gold','gate']]
 assert a['ports']==b['ports']and set(a['cells'])==set(b['cells'])
 inputs=a['ports']['a']['bits'];checks=0
 for n,c in a['cells'].items():
  other=b['cells'][n]
  assert c['connections']['Y']==other['connections']['Y']
  if c.get('attributes',{}).get('keep','').endswith('1'):assert c==other
  for assignment in range(16):
   values={bit:(assignment>>i)&1 for i,bit in enumerate(inputs)}
   def value(cell):
    index=sum((values[bit]if isinstance(bit,int)else int(bit))<<i for i,bit in enumerate(cell['connections']['A']))
    return(int(cell['parameters']['LUT'],2)>>index)&1
   assert value(c)==value(other);checks+=1
 if not selected:assert a==b
 report['cases'][name]={'status':'PASS_EXHAUSTIVE_ALL_INPUTS','truth_comparisons':checks,'kept_cells_preserved':True,'partial_design_unchanged':not selected}
 print(name,checks,'PASS',flush=True)
report['status']='PASS';(O/'summary.json').write_text(json.dumps(report,indent=2))
