#include <iridium/spirv.hpp>
#include <cstdio>
#include <cstdlib>
#include <exception>
int main(int argc,char**argv){
 if(argc!=2)return 2;unsigned failures=0;
 for(bool invalid:{false,true})for(bool extract:{false,true}){
  Iridium::SPIRV::Builder b;b.setVersion(1,5);b.requireCapability(Iridium::SPIRV::Capability::Shader);b.setAddressingModel(Iridium::SPIRV::AddressingModel::Logical);b.setMemoryModel(Iridium::SPIRV::MemoryModel::GLSL450);
  auto ft=b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::FloatTag{},32));auto it=b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::IntegerTag{},32,true));auto st=b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::StructureTag{},{{ft,0,{}},{it,4,{}}},8,4));auto whole=b.declareUndefinedValue(st);auto value=invalid?b.declareConstantScalar<int32_t>(7):b.declareConstantScalar<float>(1.5);
  auto vt=b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::VoidTag{}));auto funType=b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::FunctionTag{},vt,{},8));auto fun=b.declareFunction(funType);b.addEntryPoint({Iridium::SPIRV::ExecutionModel::Vertex,fun.id,"authored",{}});b.beginFunction(fun.id);
  bool rejected=false;try{if(extract)b.encodeCompositeExtract(invalid?it:ft,whole,{0});else b.encodeCompositeInsert(st,value,whole,{0});}catch(const std::exception&){rejected=true;}
  bool pass=rejected==invalid;failures+=!pass;std::printf("%s %s %s\n",pass?"PASS":"FAIL",extract?"extract":"insert",invalid?"mismatch":"valid");
  if(!invalid){b.encodeReturn();b.endFunction();size_t size;void*bytes=b.finalize(size);std::string path=std::string(argv[1])+(extract?"-extract.spv":"-insert.spv");FILE*f=std::fopen(path.c_str(),"wb");if(!f)return 3;std::fwrite(bytes,1,size,f);std::fclose(f);std::free(bytes);}
 }
 for(unsigned kind:{0u,1u,2u})for(bool invalid:{false,true}){
  Iridium::SPIRV::Builder b;b.setVersion(1,5);b.requireCapability(Iridium::SPIRV::Capability::Shader);b.setAddressingModel(Iridium::SPIRV::AddressingModel::Logical);b.setMemoryModel(Iridium::SPIRV::MemoryModel::GLSL450);
  auto ft=b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::FloatTag{},32));auto vec=b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::VectorTag{},2,ft,8,8));auto aggregate=kind==0?vec:kind==1?b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::ArrayTag{},ft,2,8,4)):b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::MatrixTag{},2,vec,16,8));
  auto st=b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::StructureTag{},{{aggregate,0,{}}},kind==2?16:8,8));auto whole=b.declareUndefinedValue(st);auto value=b.declareConstantScalar<float>(1.5);
  auto vt=b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::VoidTag{}));auto funType=b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::FunctionTag{},vt,{},8));auto fun=b.declareFunction(funType);b.addEntryPoint({Iridium::SPIRV::ExecutionModel::Vertex,fun.id,"authored",{}});b.beginFunction(fun.id);
  std::vector<uint32_t> indices={0,invalid?2u:1u};if(kind==2)indices.push_back(1);
  bool inserted=false,extracted=false;try{b.encodeCompositeInsert(st,value,whole,indices);}catch(const std::exception&){inserted=true;}try{b.encodeCompositeExtract(ft,whole,indices);}catch(const std::exception&){extracted=true;}
  bool pass=inserted==invalid&&extracted==invalid;failures+=!pass;std::printf("%s nested kind=%u %s\n",pass?"PASS":"FAIL",kind,invalid?"bounds":"valid");
  if(!invalid){b.encodeReturn();b.endFunction();size_t size;void*bytes=b.finalize(size);std::string path=std::string(argv[1])+"-nested-"+std::to_string(kind)+".spv";FILE*f=std::fopen(path.c_str(),"wb");if(!f)return 3;std::fwrite(bytes,1,size,f);std::fclose(f);std::free(bytes);}
 }
 {
  Iridium::SPIRV::Builder b;auto ft=b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::FloatTag{},32));auto st=b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::StructureTag{},{{ft,0,{}}},4,4));auto whole=b.declareUndefinedValue(st);auto vt=b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::VoidTag{}));auto funType=b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::FunctionTag{},vt,{},8));auto fun=b.declareFunction(funType);b.beginFunction(fun.id);
  for(bool extract:{false,true}){bool rejected=false;try{if(extract)b.encodeCompositeExtract(st,whole,{});else b.encodeCompositeInsert(st,whole,whole,{});}catch(const std::exception&){rejected=true;}failures+=!rejected;std::printf("%s %s empty indices\n",rejected?"PASS":"FAIL",extract?"extract":"insert");}
 }
 return failures?1:0;
}
