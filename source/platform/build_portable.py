"""Fresh CMake generation plus the frozen native compilation recipe.

Usage: python source/platform/build_portable.py --llvm LLVM_ROOT --out NEW_DIR
The package includes CMake/Ninja/winflexbison; LLVM is an explicit dependency.
No previously generated parser, matcher, object or build cache is consumed.
"""
from pathlib import Path
import argparse,concurrent.futures,hashlib,json,os,shutil,subprocess,tempfile,sys

def main():
 root=Path(__file__).resolve().parents[2]
 ap=argparse.ArgumentParser();ap.add_argument('--llvm',type=Path,required=True);ap.add_argument('--out',type=Path,required=True);args=ap.parse_args()
 out=args.out.resolve();assert not out.exists(),'Output must be new';out.mkdir(parents=True)
 llvm=args.llvm.resolve();cc=llvm/'bin/clang++.exe';assert cc.is_file()
 tools=root/'source/platform/build-tools';cmake=tools/'cmake/bin/cmake.exe';ninja=tools/'ninja/ninja.exe';flex=tools/'winflexbison/win_flex.exe';bison=tools/'winflexbison/win_bison.exe'
 assert all(p.is_file()for p in [cmake,ninja,flex,bison])
 assert (root/'source/platform/windows-utf8.manifest').is_file(),'Missing UTF-8 executable manifest'
 sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
 env=dict(os.environ);env['PATH']=str(llvm/'bin')+os.pathsep+env.get('PATH','');env['PYTHONUTF8']='1'
 version=subprocess.check_output([str(cc),'--version'],env=env).decode('utf-8','replace');assert '23.1.4'in version,'This reproducibility recipe pins LLVM 23.1.4'
 # Verify the declared standard support before bypassing the crashing ABI probe.
 probe=out/'compiler_probe.cc';probe.write_text('static_assert(__cplusplus >= 202002L); int main(){return 0;}\n')
 subprocess.run([str(cc),'-std=c++20',str(probe),'-o',str(out/'compiler_probe.exe')],env=env,check=True)
 work=Path(tempfile.mkdtemp(prefix='fpga-r50-build-'));src=work/'source';shutil.copytree(root/'source',src,ignore=shutil.ignore_patterns('build-tools'))
 generated=work/'cmake-build';features=work/'verified-clang.cmake';features.write_text('set(CMAKE_CXX_COMPILE_FEATURES cxx_std_11 cxx_std_14 cxx_std_17 cxx_std_20)\nset(CMAKE_CXX20_COMPILE_FEATURES cxx_std_20)\n')
 command=[str(cmake),'-S',str(src),'-B',str(generated),'-G','Ninja','-DCMAKE_BUILD_TYPE=Release','-DCMAKE_C_COMPILER='+str(llvm/'bin/clang.exe'),'-DCMAKE_CXX_COMPILER='+str(cc),'-DCMAKE_MAKE_PROGRAM='+str(ninja),'-DFLEX_EXECUTABLE='+str(flex),'-DBISON_EXECUTABLE='+str(bison),'-DPython3_EXECUTABLE='+sys.executable,'-DYOSYS_ABC_EXECUTABLE='+str(root/'yosys-abc.exe'),'-DYOSYS_WITHOUT_SLANG=ON','-DYOSYS_WITHOUT_ZLIB=ON','-DYOSYS_WITHOUT_LIBFFI=ON','-DYOSYS_WITHOUT_READLINE=ON','-DYOSYS_WITHOUT_EDITLINE=ON','-DYOSYS_WITHOUT_TCL=ON','-DYOSYS_ENABLE_UNIT_TESTS=OFF','-DBUILD_SHARED_LIBS=OFF','-DYOSYS_COMPONENTS=synth_intel;write_json;write_rtlil;read_rtlil','-DYOSYS_USE_BUNDLED_LIBS=ON','-DCMAKE_C_COMPILER_FORCED=TRUE','-DCMAKE_CXX_COMPILER_FORCED=TRUE','-DCMAKE_C_COMPILER_ID=Clang','-DCMAKE_CXX_COMPILER_ID=Clang','-DCMAKE_C_COMPILER_VERSION=23.1.4','-DCMAKE_CXX_COMPILER_VERSION=23.1.4','-DCMAKE_SIZEOF_VOID_P=8','-DCMAKE_CXX_STANDARD_COMPUTED_DEFAULT=17','-DCMAKE_CXX_EXTENSIONS_COMPUTED_DEFAULT=ON','-DCMAKE_C_STANDARD_COMPUTED_DEFAULT=17','-DCMAKE_C_EXTENSIONS_COMPUTED_DEFAULT=ON','-DCMAKE_TRY_COMPILE_PLATFORM_VARIABLES=CMAKE_CXX_STANDARD_COMPUTED_DEFAULT;CMAKE_CXX_EXTENSIONS_COMPUTED_DEFAULT;CMAKE_C_STANDARD_COMPUTED_DEFAULT;CMAKE_C_EXTENSIONS_COMPUTED_DEFAULT','-DCMAKE_DISABLE_FIND_PACKAGE_Dlfcn=TRUE','-DCMAKE_PROJECT_yosys_INCLUDE='+str(features)]
 (out/'configure-command.json').write_text(json.dumps(command,indent=2))
 with(out/'configure.log').open('wb')as f:subprocess.run(command,env=env,stdout=f,stderr=subprocess.STDOUT,check=True)
 targets=[]
 for line in(generated/'build.ninja').read_text(encoding='utf-8').splitlines():
  if line.startswith('build ')and ': CUSTOM_COMMAND'in line:
   names=line[6:].split(': CUSTOM_COMMAND')[0].split(' | ')[0].split()
   if any(x.endswith(('_pm.h','simcells_help.inc','simlib_help.inc','verilog_lexer.cc','verilog_parser.tab.cc'))for x in names):targets.append(names[0])
 with(out/'generators.log').open('wb')as f:subprocess.run([str(ninja),'-C',str(generated),'-j2',*targets],env=env,stdout=f,stderr=subprocess.STDOUT,check=True)
 # Regenerated inputs only. Disabled Slang outputs are not copied or consumed.
 g=out/'generated';g.mkdir()
 for relative in json.loads((root/'source/platform/generated-required.json').read_text()):
  p=generated/relative;assert p.is_file(),relative;dest=g/relative;dest.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(p,dest)
 objects=out/'objects';objects.mkdir();commands=[]
 for i,template in enumerate(json.loads((root/'source/platform/commands-template.json').read_text())):
  cmd=[s.replace('@SOURCE@',str(src)).replace('@GENERATED@',str(g)).replace('@LLVM@',str(llvm)).replace('@OBJECT@',str(objects/(str(i)+'.obj')))for s in template]
  commands.append(cmd)
 (out/'commands.json').write_text(json.dumps(commands,indent=2))
 def compile_one(cmd):
  obj=Path(cmd[cmd.index('-o')+1]);assert not obj.exists()
  with obj.with_suffix('.log').open('wb')as f:subprocess.run(cmd,env=env,stdout=f,stderr=subprocess.STDOUT,check=True)
 with concurrent.futures.ThreadPoolExecutor(max_workers=4)as pool:list(pool.map(compile_one,commands))
 rsp=out/'link.rsp';rsp.write_text('\n'.join('"'+Path(cmd[cmd.index('-o')+1]).as_posix()+'"'for cmd in commands))
 runtime=out/'program';runtime.mkdir()
 with(out/'link.log').open('wb')as f:subprocess.run([str(cc),'@'+str(rsp),'-o',str(runtime/'yosys.exe'),'-static','-municode','-lpsapi','-lws2_32'],env=env,stdout=f,stderr=subprocess.STDOUT,check=True)
 subprocess.run([sys.executable,str(root/'source/platform/embed_manifest.py'),str(runtime/'yosys.exe')],check=True)
 shutil.copy2(root/'yosys-abc.exe',runtime/'yosys-abc.exe');shutil.copytree(root/'share',runtime/'share')
 report={'status':'PASS_FRESH_CONFIG_GENERATORS_AND_160_NEW_OBJECTS','compiler_sha256':sha(cc),'compiler_version':version,'object_count':len(commands),'program_sha256':sha(runtime/'yosys.exe'),'temporary_source':str(src),'generated_sha256':{str(p.relative_to(g)):sha(p)for p in g.rglob('*')if p.is_file()},'objects_sha256':{p.name:sha(p)for p in objects.glob('*.obj')},'notes':['Compiler ABI auto-probe bypassed using verified compiler facts; compiler and C++20 probe actually executed','Build date/time and provenance paths can change binary hash; run package graph verifier against rebuilt output separately']}
 (out/'report.json').write_text(json.dumps(report,indent=2),encoding='utf-8');print('FRESH_BUILD_PASS',runtime/'yosys.exe',flush=True)
if __name__=='__main__':main()
