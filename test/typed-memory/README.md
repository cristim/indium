The authored builder fixture exercises valid and mismatched float/int load/store calls. It relies on type metadata recorded by the builder when creating global variables and scalar constants; callers do not supply annotations.

```sh
c++ -std=c++17 -Iinclude -Iprivate-include test/typed-memory/typed-memory.cpp src/iridium/spirv.cpp -o /tmp/typed-memory
/tmp/typed-memory /tmp/authored-memory
spirv-val --target-env vulkan1.2 /tmp/authored-memory-load.spv
spirv-val --target-env vulkan1.2 /tmp/authored-memory-store.spv
```

Baseline: valid calls pass but both invalid calls are accepted, fixture exits 1. Candidate: all four checks pass, fixture exits 0. Valid load/store modules validate and are byte-identical before/after. Errors contain only fixed coarse categories; no shader IDs, instructions or expressions are recorded.

Specification: Khronos SPIR-V OpLoad result type equals the pointer pointee type; OpStore object's type equals pointer pointee type. Existing Builder type objects and result-type map supply the checks; missing metadata is an explicit error, never an invented type.
