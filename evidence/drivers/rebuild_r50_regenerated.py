from pathlib import Path
import json,subprocess,os,hashlib
R=Path(__file__).resolve().parent
V=R/'continuation-20261030/r48-one-snapshot-clean'
O=R/'continuation-20261030/next-plan-execution/r50-regenerated-final'
cmds=json.loads((V/'build/commands.json').read_text());B=O/'clean-build';assert not B.exists();B.mkdir();commands=[]
for i,c in enumerate(cmds):
 cmd=[x.replace(str(V/'source'),str(O/'source')).replace(str(V/'generated'),str(O/'generated'))for x in c['command']];obj=B/(str(i)+'.obj');cmd[cmd.index('-o')+1]=str(obj);commands.append({'command':cmd,'object':str(obj)})
(B/'commands.json').write_text(json.dumps(commands,indent=2))
env=dict(os.environ);env['PATH']=str(Path(commands[0]['command'][0]).parent)+os.pathsep+env['PATH']
import concurrent.futures

def compile_one(c):
 with Path(c['object']).with_suffix('.log').open('wb')as log:subprocess.run(c['command'],stdout=log,stderr=subprocess.STDOUT,env=env,check=True)
 print('compiled',Path(c['command'][c['command'].index('-c')+1]).name,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=4)as pool:list(pool.map(compile_one,commands))
(B/'link.rsp').write_text('\n'.join('"'+Path(c['object']).as_posix()+'"'for c in commands))
exe=B/'yosys.exe'
with(B/'link.log').open('wb')as log:subprocess.run([commands[0]['command'][0],'@'+str(B/'link.rsp'),'-o',str(exe),'-static','-municode','-lpsapi','-lws2_32'],env=env,stdout=log,stderr=subprocess.STDOUT,check=True)
subprocess.run(['python',str(R.parents[1]/'提交材料/source/platform/embed_manifest.py'),str(exe)],check=True)
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
report={'status':'CLEAN160_COMPILED_FROZEN_GENERATED_INPUTS','no_generator_regeneration':False,'prototype_sha256':sha(O/'program/yosys.exe'),'clean_sha256':sha(exe),'binary_identical':sha(exe)==sha(O/'program/yosys.exe')}
(B/'report.json').write_text(json.dumps(report,indent=2));print(report,flush=True)
