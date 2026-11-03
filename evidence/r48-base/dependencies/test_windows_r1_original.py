from pathlib import Path
import subprocess,json,re,hashlib,os
R=Path(__file__).resolve().parent;O=R/'execution-20261030/r1-original-boundaries';O.mkdir(parents=True,exist_ok=True);N=R/'native/windows-r1-xor-complete/yosys.exe';Y=R.parents[2]/'oss-cad-suite/bin/yosys.exe'
env=dict(os.environ);env['PATH']=str(Y.parent)+os.pathsep+str(Y.parent.parent/'lib')+os.pathsep+env['PATH']
base="""module t(input [1:0] sel,input [2:0] a,b,c,d,k, input clk,output reg [2:0] y);
always @* case(sel)
0:y=a^k;1:y=b^k;2:y=c^k;default:y=d^k;
endcase
endmodule
"""
cases={'decode_default':(base,1),'alias':(base.replace('always @*','wire [2:0] alias_k=k;\nalways @*').replace('^k','^alias_k'),1),'output_keep':(base.replace('output reg [2:0] y','(* keep *) output reg [2:0] y'),0),'output_init_alias':(base.replace('always @*','(* init=3\'b101 *) wire [2:0] ay=y;\nalways @*'),0),'explicit_ff_init':(base.replace('always @*',"reg [2:0] q=3'b101; always @(posedge clk) q<=k;\nalways @*").replace('^k','^q'),1),'shared_source_user':(base.replace('output reg [2:0] y','output [2:0] extra,output reg [2:0] y').replace('always @*','assign extra=a^k;\nalways @*').replace('0:y=a^k','0:y=extra'),0),'default_mismatch':(base.replace('default:y=d^k','default:y=d|k'),0),'free_multihot':("""module t(input [2:0] s,input [2:0] a,b,c,d,k,output [2:0] y);
wire [2:0] x0=a^k,x1=b^k,x2=c^k,x3=d^k;
\$pmux #(.WIDTH(3),.S_WIDTH(3)) p(.A(x0),.B({x3,x2,x1}),.S(s),.Y(y));
endmodule
""",0)}
report=json.loads((O/'summary.json').read_text()) if (O/'summary.json').exists() else {}
for name,(v,want) in cases.items():
 if name in report:continue
 w=O/name
 if w.exists():w.rename(O/(name+'-invalid-attribute-fixture'))
 w.mkdir();(w/'input.v').write_text(v);(w/'export.ys').write_text('read_verilog -icells input.v\nhierarchy -top t\nproc\nopt_clean\n'+("setattr -set init 3'b101 w:ay\n" if name=='output_init_alias' else '')+'write_rtlil gold.il\nwrite_json gold.json\npmux_xor_factor\nwrite_rtlil gate.il\nwrite_json gate.json\n')
 with(w/'native.log').open('wb')as f:p=subprocess.run([str(N),'-Q','-s','export.ys'],cwd=w,stdout=f,stderr=subprocess.STDOUT)
 assert p.returncode==0,name
 log=(w/'native.log').read_text(errors='replace');match=re.search(r'PMUX_XOR_FACTOR: matched=(\d+) gross_removed_xor_bits=(\d+)',log);assert match;assert int(match[1])==want,(name,match[1],want)
 proof='read_rtlil gold.il\nrename t gold\nread_rtlil gate.il\nrename t gate\nequiv_make gold gate equiv\nhierarchy -top equiv\nequiv_simple -seq 3\nequiv_status -assert\n';(w/'proof.ys').write_text(proof)
 with(w/'sat.log').open('wb')as f:p=subprocess.run([str(Y),'-Q','-s','proof.ys'],cwd=w,env=env,stdout=f,stderr=subprocess.STDOUT)
 assert p.returncode==0,(name,(w/'sat.log').read_text(errors='replace')[-1200:])
 g=json.loads((w/'gold.json').read_text());h=json.loads((w/'gate.json').read_text());init=lambda x:{n:d.get('attributes',{}).get('init')for n,d in x['modules']['t']['netnames'].items()if'init'in d.get('attributes',{})};assert init(g)==init(h)
 def state(x):
  mod=x['modules']['t'];names={}
  for nn,nd in mod['netnames'].items():
   if not nd.get('hide_name'):
    for i,bit in enumerate(nd['bits']):names.setdefault(bit,[]).append((nn,i))
  return {n:{'type':d['type'],'parameters':d['parameters'],'connections':{pn:[sorted(names[bit])[0] if bit in names else bit for bit in bits]for pn,bits in d['connections'].items()}}for n,d in mod['cells'].items()if d['type']in ['$dff','$adff']}
 assert state(g)==state(h)

 report[name]={'matched':int(match[1]),'gross_removed_xor_bits':int(match[2]),'independent_sat_pass':True,'state_and_init_unchanged':True};print(name,'PASS',match[1],flush=True);(O/'summary.json').write_text(json.dumps(report,indent=2))


