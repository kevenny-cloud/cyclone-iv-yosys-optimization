from pathlib import Path
import subprocess,json,struct,os
R=Path.cwd();O=R/'execution-20261030/r10-transport-negative';O.mkdir(exist_ok=True);s=(R/'source-public9-r10-box-transport/abc9_exe.cc').read_text();helper=s[s.index('struct PackedXaiger'):s.index('void abc9_module(')];code='''#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iterator>
#include <stdexcept>
#include <cstdint>
[[noreturn]] void log_error(const char *s,...) {throw std::runtime_error(s);}
void log(const char *,...) {}
'''+helper+'''int main(int argc,char **argv){try{preserve_packed_box_hierarchy(argv[1],argv[2],argc<4);return 0;}catch(const std::exception&){return 2;}}
''';(O/'parser-test.cc').write_text(code);T=R/'windows-build-tools/llvm-mingw-20261020-ucrt-x86_64/bin';env=dict(os.environ);env['PATH']=str(T)+os.pathsep+env['PATH'];subprocess.run([str(T/'clang++.exe'),'-std=c++20',str(O/'parser-test.cc'),'-static','-o',str(O/'parser-test.exe')],env=env,check=True)
word=lambda n:struct.pack('>I',n);payload=b''.join(word(n)for n in [1,2,1,1,0,1,1,1,1,0]);h=b'h'+word(len(payload))+payload;prefix=b'aig 2 2 0 1 0\n2\nc\n';good=prefix+h;empty=prefix
cases={'valid':(good,empty,False,0),'disabled_verify':(good,empty,True,2),'interface_mismatch':(good,b'aig 3 3 0 1 0\n2\nc\n',False,2),'bad_version':(prefix+b'h'+word(len(payload))+word(2)+payload[4:],empty,False,2),'truncated_record':(good[:-3],empty,False,2),'bad_box_count':(prefix+h[:25]+word(2)+h[29:],empty,False,2),'truncated_varint':(b'aig 3 2 0 1 1\n2\n\x80',empty,False,2)};out={}
for name,(before,after,disabled,expected)in cases.items():
 (O/'before.aig').write_bytes(before);(O/'after.aig').write_bytes(after);p=subprocess.run([str(O/'parser-test.exe'),'before.aig','after.aig']+(['noverify']if disabled else[]),cwd=O);assert p.returncode==expected,(name,p.returncode);out[name]={'returncode':p.returncode,'pass':True}
 if name=='valid':assert (O/'after.aig').read_bytes()==good
(O/'summary.json').write_text(json.dumps(out,indent=2));print('PASS',len(out))
