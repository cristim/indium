The authored builder fixture checks inserted/extracted member types and nested vector, array and matrix bounds, including empty index rejection. It uses only our declared type objects and builder calls.

```sh
c++ -std=c++17 -Iinclude -Iprivate-include test/typed-composite/typed-composite.cpp src/iridium/spirv.cpp -o /tmp/typed-composite
/tmp/typed-composite /tmp/authored-composite
for module in /tmp/authored-composite-*.spv; do spirv-val --target-env vulkan1.2 "$module" || exit; done
```

Baseline: seven invalid cases are accepted, fixture exits 1. Candidate: all twelve checks pass, fixture exits 0. Five valid modules validate and are byte-identical before/after. Ten authored texture-sampling controls also validate.

The fixed error categories contain no shader IDs, instructions or expressions. Khronos SPIR-V OpCompositeInsert/OpCompositeExtract require selected member types to match their object/result type and all indices to be in bounds. The existing Builder type schema provides the traversal; no guest shader input is inspected.
