from pathlib import Path
import json,hashlib,shutil,subprocess,concurrent.futures,os
R=Path(__file__).resolve().parent;P=R/'validation-r10-20261030';V=R/'continuation-20261030/r48-one-snapshot-clean';S=V/'source';G=V/'generated';B=V/'build';N=V/'program';T=R/'windows-build-tools/llvm-mingw-20261020-ucrt-x86_64/bin';sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();assert not V.exists();V.mkdir();shutil.copytree(P/'source',S);shutil.copytree(P/'generated',G)
for p in (R/'source-public9-r48-one-snapshot').iterdir():
 if p.suffix not in ['.cc','.h','.v','.txt']:continue
 sub='passes/techmap'if p.name in ['abc.cc','abc9.cc','abc9_exe.cc']else'techlibs/intel/cycloneiv'if p.name=='cells_map.v'else'techlibs/intel/common'if p.name=='ff_map_sync.v'else'techlibs/intel';q=S/sub/('CMakeLists.txt'if p.name=='intel_CMakeLists.txt'else p.name);shutil.copy2(p,q)
plan={'status':'FROZEN_BEFORE_BUILD','parent':str(P),'changes':str(R/'continuation-20261030/r48-plan.json'),'source_sha256':{str(p.relative_to(S)):sha(p)for p in S.rglob('*')if p.is_file()},'generated_sha256':{str(p.relative_to(G)):sha(p)for p in G.rglob('*')if p.is_file()},'formal_submission_sha256':sha(R.parents[1]/'提交材料/yosys.exe')};(V/'plan.json').write_text(json.dumps(plan,indent=2),encoding='utf-8');B.mkdir();(B/'objects').mkdir();N.mkdir();cmds=json.loads((P/'build/commands.json').read_text());commands=[]
for i,item in enumerate(cmds):
 cmd=[a.replace(str(P/'source'),str(S)).replace(str(P/'generated'),str(G))for a in item['command']];obj=B/'objects'/f'{i:03d}.obj';cmd[cmd.index('-o')+1]=str(obj);commands.append({'source':cmd[cmd.index('-c')+1],'object':str(obj),'command':cmd})
(B/'commands.json').write_text(json.dumps(commands,indent=2),encoding='utf-8');env=dict(os.environ);env['PATH']=str(T)+os.pathsep+env['PATH'];report={'status':'BUILDING','completed':0,'expected_objects':len(commands),'clean_rebuild':True,'no_prior_objects_or_build_tree':True,'compiler_sha256':sha(T/'clang++.exe')}
def run(x):
 obj=Path(x['object']);assert not obj.exists()
 with(obj.with_suffix('.log')).open('wb')as f:subprocess.run(x['command'],env=env,stdout=f,stderr=subprocess.STDOUT,check=True)
 print('compiled',Path(x['source']).name,flush=True)
with concurrent.futures.ThreadPoolExecutor(max_workers=4)as pool:
 for x in pool.map(run,commands):report['completed']+=1
rsp=B/'link.rsp';rsp.write_text('\n'.join('"'+Path(x['object']).as_posix()+'"'for x in commands),encoding='utf-8')
with(B/'link.log').open('wb')as f:subprocess.run([str(T/'clang++.exe'),'@'+str(rsp),'-o',str(N/'yosys.exe'),'-static','-municode','-lpsapi','-lws2_32'],env=env,stdout=f,stderr=subprocess.STDOUT,check=True)
shutil.copy2(P/'program/yosys-abc.exe',N/'yosys-abc.exe');shutil.copytree(P/'program/share',N/'share');subprocess.run(['python',str(R.parents[1]/'提交材料/source/platform/embed_manifest.py'),str(N/'yosys.exe')],check=True)
report.update(status='BUILT_REQUIRES_VALIDATION',candidate_sha256=sha(N/'yosys.exe'),abc_sha256=sha(N/'yosys-abc.exe'),fresh_objects=len(list((B/'objects').glob('*.obj'))),object_sha256={x['object']:sha(Path(x['object']))for x in commands});(B/'report.json').write_text(json.dumps(report,indent=2),encoding='utf-8');shutil.copytree(N,R/'native/windows-r48-one-snapshot-clean');print('BUILT_CLEAN160',flush=True)
