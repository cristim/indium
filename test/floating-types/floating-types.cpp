#include <iridium/spirv.hpp>
#include <cstdio>
#include <cstdlib>
#include <exception>
using namespace Iridium::SPIRV;
int main(int argc,char**argv){
 if(argc!=2)return 2;unsigned failures=0,serial=0;
 for(bool vector:{false,true})for(int op=0;op<5;++op)for(bool invalid:{false,true}){
  Builder b;b.setVersion(1,5);b.requireCapability(Capability::Shader);b.requireCapability(Capability::Float16);b.setAddressingModel(AddressingModel::Logical);b.setMemoryModel(MemoryModel::GLSL450);
  auto f=b.declareType(Type(Type::FloatTag{},32));auto h=b.declareType(Type(Type::FloatTag{},16));auto i=b.declareType(Type(Type::IntegerTag{},32,true));
  auto shape=[&](ResultID t){return vector?b.declareType(Type(Type::VectorTag{},4,t,16,8)):t;};
  auto ft=shape(f),ht=shape(h),it=shape(i);auto value=b.declareNullValue(ft);auto other=b.declareNullValue(invalid?it:ft);
  auto vt=b.declareType(Type(Type::VoidTag{}));auto funType=b.declareType(Type(Type::FunctionTag{},vt,{},8));auto fun=b.declareFunction(funType);b.addEntryPoint({ExecutionModel::Vertex,fun.id,"authored",{}});b.beginFunction(fun.id);
  bool rejected=false;try{if(op==4)b.encodeFConvert(invalid?ft:ht,value);else {Opcode ops[]={Opcode::FAdd,Opcode::FSub,Opcode::FMul,Opcode::FDiv};b.encodeArithBinop(ops[op],ft,value,other);}}catch(const std::exception&){rejected=true;}
  bool pass=rejected==invalid;failures+=!pass;std::printf("%s %s operation%d %s\n",pass?"PASS":"FAIL",vector?"vector":"scalar",op,invalid?"mismatch":"valid");
  if(!invalid){b.encodeReturn();b.endFunction();size_t size;void*bytes=b.finalize(size);auto path=std::string(argv[1])+"-"+std::to_string(serial++)+".spv";FILE*fp=std::fopen(path.c_str(),"wb");if(!fp)return 3;std::fwrite(bytes,1,size,fp);std::fclose(fp);std::free(bytes);}
 }
 for(int mismatch=0;mismatch<4;++mismatch){
  Builder b;auto f=b.declareType(Type(Type::FloatTag{},32));auto h=b.declareType(Type(Type::FloatTag{},16));auto i=b.declareType(Type(Type::IntegerTag{},32,true));
  auto fv=b.declareType(Type(Type::VectorTag{},4,f,16,8));auto hv=b.declareType(Type(Type::VectorTag{},2,h,4,4));
  auto source=mismatch==0?i:(mismatch==1?f:fv);auto destination=mismatch==0?h:(mismatch==1?hv:(mismatch==2?h:hv));
  auto value=b.declareNullValue(source);auto vt=b.declareType(Type(Type::VoidTag{}));auto funType=b.declareType(Type(Type::FunctionTag{},vt,{},8));auto fun=b.declareFunction(funType);b.beginFunction(fun.id);
  bool rejected=false;try{b.encodeFConvert(destination,value);}catch(const std::exception&){rejected=true;}
  failures+=!rejected;std::printf("%s conversion boundary%d\n",rejected?"PASS":"FAIL",mismatch);b.encodeReturn();b.endFunction();
 }
 return failures?1:0;
}
