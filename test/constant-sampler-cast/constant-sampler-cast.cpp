#include "../../src/iridium/air.cpp"
#include <cstdio>
int main() {
    if (!Iridium::DynamicLLVM::init()) return 2;
    LLVMContextRef context = LLVMContextCreate();
    LLVMModuleRef module = LLVMModuleCreateWithNameInContext("authored-opaque-resource-cast", context);
    LLVMValueRef global = LLVMAddGlobalInAddressSpace(module, LLVMInt64TypeInContext(context), "resource", 2);
    LLVMValueRef cast = LLVMConstAddrSpaceCast(global, LLVMPointerTypeInContext(context, 0));
    if (LLVMGetValueKind(cast) != LLVMConstantExprValueKind || LLVMGetConstOpcode(cast) != LLVMAddrSpaceCast) return 3;
    Iridium::SPIRV::Builder builder;
    using Type = Iridium::SPIRV::Type;
    using Storage = Iridium::SPIRV::StorageClass;
    auto sampler = builder.declareType(Type(Type::SamplerTag {}));
    auto pointer = builder.declareType(Type(Type::PointerTag {}, Storage::UniformConstant, sampler, 8));
    auto resource = builder.addGlobalVariable(pointer, Storage::UniformConstant);
    builder.associateExistingResultID(resource, reinterpret_cast<uintptr_t>(global));
    builder.setResultType(resource, pointer);
    int failures = 0;
    try {
        auto result = llvmValueToResultID(builder, cast);
        if (result != resource || builder.lookupResultType(result) != pointer) ++failures;
        else std::puts("PASS opaque sampler cast preserves binding and type");
    } catch (const std::exception& error) { std::fprintf(stderr,"FAIL sampler cast: %s\n",error.what()); ++failures; }
    if (llvmValueToResultID(builder, global) != resource) ++failures;
    LLVMValueRef roundtrip = LLVMConstIntToPtr(LLVMConstPtrToInt(global, LLVMInt64TypeInContext(context)), LLVMPointerTypeInContext(context, 0));
    try {
        if (llvmValueToResultID(builder, roundtrip) != resource) ++failures;
        else std::puts("PASS opaque sampler pointer roundtrip preserves binding");
    } catch (const std::exception& error) { std::fprintf(stderr,"FAIL sampler roundtrip: %s\n",error.what()); ++failures; }
    LLVMValueRef literal = LLVMConstInt(LLVMInt64TypeInContext(context), 41, 0);
    builder.associateResourceResultID(resource, reinterpret_cast<uintptr_t>(literal));
    LLVMValueRef literalCast = LLVMConstIntToPtr(literal, LLVMPointerTypeInContext(context, 0));
    try {
        if (llvmValueToResultID(builder, literalCast) != resource || llvmValueToResultID(builder, literal) == resource) ++failures;
        else std::puts("PASS registered resource literal preserves binding without changing numeric constants");
    } catch (const std::exception& error) { std::fprintf(stderr,"FAIL resource literal: %s\n",error.what()); ++failures; }
    auto scalar = builder.declareType(Type(Type::IntegerTag {}, 64, true));
    auto scalarPointer = builder.declareType(Type(Type::PointerTag {}, Storage::UniformConstant, scalar, 8));
    auto other = builder.addGlobalVariable(scalarPointer, Storage::UniformConstant);
    LLVMValueRef otherGlobal = LLVMAddGlobalInAddressSpace(module, LLVMInt64TypeInContext(context), "other", 2);
    LLVMValueRef otherCast = LLVMConstAddrSpaceCast(otherGlobal, LLVMPointerTypeInContext(context, 0));
    builder.associateExistingResultID(other, reinterpret_cast<uintptr_t>(otherGlobal));
    builder.setResultType(other, scalarPointer);
    bool rejected = false;
    try { llvmValueToResultID(builder, otherCast); } catch (const std::exception&) { rejected = true; }
    if (!rejected) { std::puts("FAIL non-sampler cast accepted"); ++failures; }
    else std::puts("PASS non-sampler cast rejected");
    LLVMDisposeModule(module); LLVMContextDispose(context);
    return failures ? 1 : 0;
}
