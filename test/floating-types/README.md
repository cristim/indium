This explicitly authored Builder fixture checks only floating arithmetic and floating width conversions. It uses no guest shader input. The four arithmetic operations require identical scalar/vector floating operand and result types; conversion requires matching scalar/vector shape, floating components and different component types, as specified by Khronos SPIR-V.

```sh
c++ -std=c++17 -Iinclude -Iprivate-include test/floating-types/floating-types.cpp src/iridium/spirv.cpp -o /tmp/floating-types
/tmp/floating-types /tmp/authored-floating
for n in $(seq 0 9); do
  spirv-val --target-env vulkan1.2 "/tmp/authored-floating-$n.spv" || exit
done
```

Baseline accepts all fourteen invalid calls and exits 1. Candidate passes all 24 checks and exits 0. The ten valid emitted modules are byte-identical before/after and validate. The extra conversion boundaries cover integer input, scalar-to-vector, vector-to-scalar and unequal vector component counts. Comparisons, shifts, Dot and other arithmetic operations are unchanged.

This is a diagnostic guard, not evidence that a guest shader has these mismatches. Missing owned result-type metadata remains an observer limitation. Provenance: the existing Builder type schema and https://registry.khronos.org/SPIR-V/specs/unified1/SPIRV.html#OpFAdd and #OpFConvert.
