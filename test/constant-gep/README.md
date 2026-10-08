The fixture authors LLVM constant GEP expressions through LLVM-C. Nonzero indices must remain constant expressions; zero is an existing registered-pointer identity control. The global is explicitly associated with a known physical pointer. This does not implement global allocation or identify an original application's expression.

From the repository root:

```sh
flock /tmp/agent-locks/darling-heavy-build.lock c++ -std=c++17 -DHOST_LLVM_LIBNAME='"libLLVM.so"' -Iinclude -Iprivate-include test/constant-gep/constant-gep.cpp src/iridium/spirv.cpp src/iridium/dynamic-llvm.cpp -ldl -lLLVM-22 -o /tmp/constant-gep
flock /tmp/agent-locks/darling-heavy-build.lock c++ -std=c++17 test/gep-index/gep-readback.cpp -lvulkan -o /tmp/gep-readback
for width in 32 64; do
  for index in 0 1 -1; do
    /tmp/constant-gep "$width" "$index" "/tmp/constant-gep-$width-$index.spv" || exit
    spirv-val --target-env vulkan1.3 "/tmp/constant-gep-$width-$index.spv" || exit
    expected=22; [ "$index" = 1 ] && expected=33; [ "$index" = -1 ] && expected=11
    VK_DRIVER_FILES=/usr/share/vulkan/icd.d/asahi_icd.json /tmp/gep-readback "/tmp/constant-gep-$width-$index.spv" "$expected" || exit
  done
done
/tmp/constant-gep 32 1 /tmp/rejected.spv logical
/tmp/constant-gep 1 1 /tmp/rejected.spv; test "$?" = 1
/tmp/constant-gep 128 1 /tmp/rejected.spv; test "$?" = 1
```

Native readback reuses the authored PR19 harness unchanged (published head a5e8f648127a0d4251eaf95432bd68193d757cc8, https://github.com/VibeDarling/indium/blob/a5e8f648127a0d4251eaf95432bd68193d757cc8/test/gep-index/gep-readback.cpp), with its safe three-float allocation and middle address. Use the appropriate ICD on another machine. Before this fix, all four retained expressions reject with unsupported LLVM value kind 10; zero identity validates and reads22. After, all six modules validate under Vulkan1.3 and native Apple M1 reads22/33/11 for both widths. The nonphysical pointer rejects; widths1/128 reject explicitly. Existing dynamic32/64 +/-1 modules remain byte-identical across the helper extraction.

Provenance: existing OSS dynamic GEP lowering and vendored LLVM-C declarations (rung2); published LLVM constant-expression/GEP semantics and LLVM22.1.8 Core.cpp LLVMGetGEPSourceElementType using GEPOperator for both instructions and expressions; entirely authored API inputs/readbacks (rung4). Host LLVM was22.1.8. Public source: https://github.com/llvm/llvm-project/blob/llvmorg-22.1.8/llvm/lib/IR/Core.cpp#L3040 and https://llvm.org/docs/LangRef.html#constant-expressions . The existing fixed64 physical-address model is preserved; other DataLayout index widths, logical pointer conversion, global allocation, and sampler expressions remain outside this change.
