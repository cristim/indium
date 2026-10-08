#include <iridium/spirv.hpp>
#include <cstdio>
#include <cstdlib>
#include <exception>
int main(int argc,char**argv){
 if(argc!=2)return 2;unsigned failures=0;
 for(bool invalid:{false,true})for(bool store:{false,true}){
  Iridium::SPIRV::Builder b;b.setVersion(1,5);b.requireCapability(Iridium::SPIRV::Capability::Shader);b.setAddressingModel(Iridium::SPIRV::AddressingModel::Logical);b.setMemoryModel(Iridium::SPIRV::MemoryModel::GLSL450);
  auto ft=b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::FloatTag{},32));auto it=b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::IntegerTag{},32,true));auto pt=b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::PointerTag{},Iridium::SPIRV::StorageClass::Output,ft,8));auto p=b.addGlobalVariable(pt,Iridium::SPIRV::StorageClass::Output);b.addDecoration(p,{Iridium::SPIRV::DecorationType::Location,{0}});
  auto value=invalid?b.declareConstantScalar<int32_t>(7):b.declareConstantScalar<float>(1.5);
  auto vt=b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::VoidTag{}));auto funType=b.declareType(Iridium::SPIRV::Type(Iridium::SPIRV::Type::FunctionTag{},vt,{},8));auto fun=b.declareFunction(funType);b.addEntryPoint({Iridium::SPIRV::ExecutionModel::Vertex,fun.id,"authored",{p}});b.beginFunction(fun.id);
  bool rejected=false;try{if(store)b.encodeStore(p,value);else b.encodeLoad(invalid?it:ft,p);}catch(const std::exception&){rejected=true;}
  bool pass=rejected==invalid;failures+=!pass;std::printf("%s %s %s\n",pass?"PASS":"FAIL",store?"store":"load",invalid?"mismatch":"valid");
  if(!invalid){b.encodeReturn();b.endFunction();size_t size;void*bytes=b.finalize(size);std::string path=std::string(argv[1])+(store?"-store.spv":"-load.spv");FILE*f=std::fopen(path.c_str(),"wb");if(!f)return 3;std::fwrite(bytes,1,size,f);std::fclose(f);std::free(bytes);}
 }
 return failures?1:0;
}
