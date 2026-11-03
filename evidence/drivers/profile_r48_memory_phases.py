"""Diagnostic only: timestamp stdout and sample main process memory, never score."""
from pathlib import Path
import hashlib,json,os,subprocess,threading,time,shutil,psutil,re
R=Path(__file__).resolve().parent
V=R/'continuation-20261030/r48-one-snapshot-clean'
P=R/'候选交付/R48-20261030'
O=R/'continuation-20261030/next-plan-execution/memory-diagnostic-v1'
assert not O.exists()
O.mkdir(parents=True)
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
fixtures=json.loads((P/'verification/fixtures.json').read_text(encoding='utf-8'))['cases']
bins={'baseline':R/'native/windows-reference/yosys.exe','candidate':V/'program/yosys.exe'}
plan={'purpose':'DIAGNOSTIC_NOT_EFFICIENCY_SCORE','sampling_seconds':0.001,'case_order':list(fixtures),'variants':{k:{'path':str(v),'sha256':sha(v)}for k,v in bins.items()},'limitations':['stdout receipt can lag execution','sampling can miss short peaks','observer affects elapsed time','historical peak belongs to entire process, not necessarily current phase']}
(O/'plan.json').write_text(json.dumps(plan,indent=2),encoding='utf-8')
report={'status':'RUNNING','cases':{}}
for name,c in fixtures.items():
 report['cases'][name]={}
 for variant,exe in bins.items():
  w=O/name/variant;w.mkdir(parents=True)
  for n,h in c['input_sha256'].items():
   src=P/'verification/fixtures'/name/n;assert sha(src)==h;shutil.copy2(src,w/n)
  (w/'run.ys').write_text(c['script'],encoding='utf-8')
  env={k:v for k,v in os.environ.items()if not k.startswith('YOSYS')and k not in ['ABC','FPGA_DIAG_FILE']}
  env['PATH']=str(exe.parent)+os.pathsep+env.get('SystemRoot','C:/Windows')+'/System32'
  start=time.perf_counter();proc=subprocess.Popen([str(exe),'-Q','-s','run.ys'],cwd=w,env=env,stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
  events=[];samples=[]
  def reader():
   with (w/'run.log').open('wb')as log:
    for line in proc.stdout:
     log.write(line);events.append([time.perf_counter()-start,line.decode('utf-8','replace').rstrip()])
  thread=threading.Thread(target=reader);thread.start();obs=psutil.Process(proc.pid)
  while proc.poll()is None:
   try:
    m=obs.memory_info();samples.append([time.perf_counter()-start,m.rss,getattr(m,'peak_wset',0)])
   except psutil.NoSuchProcess:break
   time.sleep(0.001)
  thread.join();assert proc.wait()==0
  assert samples
  (w/'events.json').write_text(json.dumps(events),encoding='utf-8');(w/'samples.json').write_text(json.dumps(samples),encoding='utf-8')
  peak=max(s[2]for s in samples);rise=[];prior=0;idx=0;phase='startup'
  for t,rss,pk in samples:
   while idx<len(events)and events[idx][0]<=t:
    line=events[idx][1]
    if re.match(r'^\d+(?:\.\d+)*\. ',line):phase=line
    idx+=1
   if pk>prior:rise.append({'time':t,'peak_bytes':pk,'observed_phase':phase});prior=pk
  cells=json.loads((w/'final.json').read_text())['modules'][c['top']]['cells']
  logic=sum(x['type']in ['cycloneiv_lcell_comb','$not']for x in cells.values())
  row={'observed_retained_peak_bytes':peak,'last_peak_rises':rise[-8:],'samples':len(samples),'logic':logic}
  report['cases'][name][variant]=row
  (O/'summary.json').write_text(json.dumps(report,indent=2),encoding='utf-8');print(name,variant,peak,logic,flush=True)
report['status']='DIAGNOSTIC_COMPLETE_NOT_SCORE'
(O/'summary.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
