#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iterator>
#include <stdexcept>
#include <cstdint>
[[noreturn]] void log_error(const char *s,...) {throw std::runtime_error(s);}
void log(const char *,...) {}
struct PackedXaiger {
 std::string bytes, tail; size_t footer; unsigned inputs, latches, outputs;
 std::vector<std::pair<char,std::string>> chunks;
 static unsigned word(const std::string &s,size_t p) {
  if(p+4>s.size()) log_error("Truncated XAIGER transport record.\n");
  return (unsigned((unsigned char)s[p])<<24)|(unsigned((unsigned char)s[p+1])<<16)|
         (unsigned((unsigned char)s[p+2])<<8)|unsigned((unsigned char)s[p+3]);
 }
 PackedXaiger(const std::string &path) {
  std::ifstream f(path,std::ios::binary);
  if(!f) log_error("Cannot read XAIGER transport snapshot.\n");
  bytes.assign(std::istreambuf_iterator<char>(f),{});
  size_t p=bytes.find('\n'); if(p==std::string::npos) log_error("Missing AIGER header.\n");
  std::istringstream h(bytes.substr(0,p)); std::string format;unsigned maximum,ands;
  if(!(h>>format>>maximum>>inputs>>latches>>outputs>>ands)||format!="aig")
   log_error("Unsupported AIGER transport header.\n");
  unsigned optional=0;while(h>>optional)if(optional)log_error("Unsupported AIGER property sections in packing transport.\n");
  ++p;
  for(uint64_t i=0;i<uint64_t(latches)+outputs;++i){p=bytes.find('\n',p);if(p==std::string::npos)log_error("Truncated AIGER output table.\n");++p;}
  for(uint64_t i=0;i<uint64_t(ands)*2;++i){unsigned n=0;while(true){if(p>=bytes.size()||++n>6)log_error("Invalid AIGER variable-length integer.\n");if(!((unsigned char)bytes[p++]&128))break;}}
  while(p<bytes.size()&&bytes[p]!='c'){p=bytes.find('\n',p);if(p==std::string::npos)log_error("Missing XAIGER extension marker.\n");++p;}
  if(p>=bytes.size())log_error("Missing XAIGER extension marker.\n");
  ++p;if(p<bytes.size()&&bytes[p]=='\n')++p;footer=p;
  while(p+5<=bytes.size()){
   char tag=bytes[p];if(std::string("mMarshino").find(tag)==std::string::npos)break;
   unsigned size=word(bytes,p+1);if(size>bytes.size()-p-5)log_error("Invalid XAIGER extension size.\n");
   chunks.push_back({tag,bytes.substr(p,5+size)});p+=5+size;
  }
  tail=bytes.substr(p);
 }
 std::string hierarchy()const{for(auto &c:chunks)if(c.first=='h')return c.second;return {};}
};
static void preserve_packed_box_hierarchy(const std::string &snapshot,const std::string &output,bool verified) {
 PackedXaiger before(snapshot);
 std::string hierarchy=before.hierarchy();
 if(hierarchy.empty())return;
 if(hierarchy.size()<29)log_error("Invalid XAIGER hierarchy record.\n");
 unsigned boxes=PackedXaiger::word(hierarchy,25);
 if(!boxes)return;
 PackedXaiger after(output);
 if(PackedXaiger::word(hierarchy,5)!=1)log_error("Unsupported XAIGER hierarchy version.\n");
 if(PackedXaiger::word(hierarchy,9)!=before.inputs+before.latches||PackedXaiger::word(hierarchy,13)!=before.outputs+before.latches)
  log_error("XAIGER hierarchy interface disagrees with the snapshot.\n");
 if(!verified)log_error("Packing box transport requires ABC9 equivalence verification.\n");
 if(before.inputs!=after.inputs||before.outputs!=after.outputs||before.latches!=after.latches)
  log_error("LUT packing changed the timing-box interface; refusing reintegration.\n");
 if(hierarchy.size()!=size_t(29)+size_t(boxes)*16)
  log_error("Invalid timing-box hierarchy payload.\n");
 std::string result=after.bytes.substr(0,after.footer)+hierarchy;
 for(auto &c:after.chunks)if(c.first!='h')result+=c.second;
 result+=after.tail;
 std::ofstream f(output,std::ios::binary|std::ios::trunc);f.write(result.data(),result.size());
 if(!f)log_error("Failed to preserve packed XAIGER hierarchy.\n");
 log("ABC9_PACK_TRANSPORT: preserved %u timing boxes; interface unchanged.\n",boxes);
}

int main(int argc,char **argv){try{preserve_packed_box_hierarchy(argv[1],argv[2],argc<4);return 0;}catch(const std::exception&){return 2;}}
