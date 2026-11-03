from pathlib import Path
import shutil,subprocess,json,hashlib
R=Path(__file__).resolve().parent;V=R/'continuation-20261030/r48-one-snapshot-clean';B=V/'boundaries';B.mkdir(exist_ok=True);RUN=V/'runners';RUN.mkdir(exist_ok=True);P=V/'program/yosys.exe';sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();groups=[]
shutil.copytree(R/'execution-20261030/r1-boundary-plan',V/'frozen-rule-inputs/xor-multiple',dirs_exist_ok=True)
for file,oldout,newout,oldexe in [
 ('test_windows_r1_original.py','execution-20261030/r1-original-boundaries','xor','native/windows-r1-xor-complete/yosys.exe'),
 ('test_windows_r1_multiple.py','execution-20261030/r1-new-boundaries','xor-multiple','native/windows-r1-xor-complete/yosys.exe'),
 ('test_windows_r3.py','execution-20261030/r3-boundaries','arithmetic','native/windows-r3-arith-share/yosys.exe')]:
 s=(R/file).read_text(encoding='utf-8-sig').replace(oldout,'continuation-20261030/r48-one-snapshot-clean/boundaries/'+newout).replace(oldexe,'continuation-20261030/r48-one-snapshot-clean/program/yosys.exe').replace('R=Path(__file__).resolve().parent','R=Path('+repr(str(R))+')').replace('execution-20261030/r1-boundary-plan','continuation-20261030/r48-one-snapshot-clean/frozen-rule-inputs/xor-multiple')
 script=RUN/file;script.write_text(s,encoding='utf-8');groups.append((newout,script))
# Keep the existing independent transition formulas and fault injection for actual emitted FF wiring.
s=(R/'check_plan_v8_gate_load.py').read_text(encoding='utf-8');marker='exec(compile(t,';at=s.index(marker);s=s[:at]+'''t=t.replace("O=R/'plan-v8-gate-load/local-checks'","O=R/'continuation-20261030/r48-one-snapshot-clean/boundaries/ff-load'").replace("Y=R/'native/plan-v8-gate-load-probe/yosys.exe'","Y=R/'continuation-20261030/r48-one-snapshot-clean/program/yosys.exe'")
t=t.replace('R=Path(__file__).resolve().parent','R=Path('+repr(str(R))+')')
'''+s[at:];s=s.replace('R=Path(__file__).resolve().parent','R=Path('+repr(str(R))+')',1);script=RUN/'ff-load.py';script.write_text(s,encoding='utf-8');groups.append(('ff-load',script))
s=(R/'test_windows_r10_clean_boxes.py').read_text(encoding='utf-8-sig').replace('execution-20261030/r10-clean-box-tests','continuation-20261030/r48-one-snapshot-clean/boundaries/boxes').replace('native/windows-r10-clean/yosys.exe','continuation-20261030/r48-one-snapshot-clean/program/yosys.exe');needle=';out={}'
extra=r'''
cases['kept_box']=head+'module top(input[3:0] a,output y);wire z;(* keep *)box1 u(.i(a[0]^a[1]),.o(z));assign y=z^(a[2]&a[3]);endmodule\n'
cases['parameter_vector_box']='''+repr('''(* abc9_box, blackbox *)module bv #(parameter P=1)(input[1:0] a,b,output[1:0] y,z);
specify(a *> y)=P;(b *> y)=P;(a *> z)=P;(b *> z)=P;endspecify endmodule
module top(input[7:0] a,output[3:0] y);wire[1:0] u,v;bv #(.P(2)) box(.a(a[1:0]^a[3:2]),.b(a[5:4]),.y(u),.z(v));assign y={u^a[7:6],v^a[3:2]};endmodule
''')+r'''
cases['shared_box_output']=head+'module top(input[3:0] a,output[1:0] y);wire z;box1 u(.i(a[0]^a[1]),.o(z));assign y={z&a[2],z^a[3]};endmodule\n'
''';assert needle in s;s=s.replace(needle,'\n'+extra+'\nout={}',1)
s=s.replace('equiv_make gold gate equiv\\nhierarchy -top equiv\\nequiv_simple\\nequiv_status -assert\\n','miter -equiv -flatten gold gate miter\\nprep -top miter\\nsat -verify -prove trigger 0 -set-def-inputs miter\\n')
script=RUN/'boxes.py';script.write_text(s,encoding='utf-8');groups.append(('boxes',script))
s=(R/'test_transport_negative.py').read_text(encoding='utf-8-sig').replace('execution-20261030/r10-transport-negative','continuation-20261030/r48-one-snapshot-clean/boundaries/transport-negative').replace('source-public9-r10-box-transport/abc9_exe.cc','continuation-20261030/r48-one-snapshot-clean/source/passes/techmap/abc9_exe.cc');script=RUN/'transport-negative.py';script.write_text(s,encoding='utf-8');groups.append(('transport-negative',script))
report={'status':'RUNNING','candidate_sha256':sha(P),'groups':{}}
try:
 for name,script in groups:
  with(RUN/(name+'.log')).open('wb')as f:p=subprocess.run(['python',str(script)],cwd=R,stdout=f,stderr=subprocess.STDOUT,timeout=180)
  if p.returncode:raise RuntimeError(name+' failed; '+str(RUN/(name+'.log')))
  data=json.loads((B/name/'summary.json').read_text(encoding='utf-8'));report['groups'][name]={'status':'PASS','runner_sha256':sha(script),'summary_sha256':sha(B/name/'summary.json'),'tests':len(data.get('tests',[]))if isinstance(data,dict)and'tests'in data else len(data)};print(name,'PASS',flush=True);(B/'summary.json').write_text(json.dumps(report,indent=2))
 report['status']='PASS'
except Exception as e:report.update(status='ERROR',error=repr(e));raise
finally:(B/'summary.json').write_text(json.dumps(report,indent=2))
