# Explicit single position output

The LLVM-C-API fixture authors every input module. It exercises the existing
`air.position` return metadata, a struct control, ordinary float4 output, and
invalid scalar/stage declarations. No application shader is decoded or inspected.

From the repository root (LLVM22 and Vulkan development packages required):
```sh
c++ -std=c++17 '-DHOST_LLVM_LIBNAME="libLLVM.so"' -Iinclude -Iprivate-include test/single-position/position-module.cpp src/iridium/spirv.cpp src/iridium/dynamic-llvm.cpp -ldl -lLLVM-22 -o /tmp/position-module
c++ -std=c++20 test/single-position/position-render.cpp -lvulkan -o /tmp/position-render
for mode in single struct fragment ordinary; do
  /tmp/position-module "$mode" "/tmp/position-$mode.spv"
  spirv-val --target-env vulkan1.3 "/tmp/position-$mode.spv"
done
/tmp/position-module invalid-type /tmp/invalid.spv # must exit1
/tmp/position-module invalid-stage /tmp/invalid.spv # must exit1
VK_DRIVER_FILES=/usr/share/vulkan/icd.d/asahi_icd.json /tmp/position-render /tmp/position-single.spv /tmp/position-fragment.spv
VK_DRIVER_FILES=/usr/share/vulkan/icd.d/asahi_icd.json /tmp/position-render /tmp/position-struct.spv /tmp/position-fragment.spv
VK_DRIVER_FILES=/usr/share/vulkan/icd.d/asahi_icd.json /tmp/position-render /tmp/position-ordinary.spv /tmp/position-fragment.spv # must exit1
```

Select an appropriate native ICD on other systems. The renderer requires Vulkan1.3
and bufferDeviceAddress, submits a fullscreen triangle and reads 300x200 exact RGBA
pixels. Before the fix, single output validates but yields 0 red pixels/exit1; struct yields
60000 red pixels/exit0. After, both yield60000 red pixels/exit0 on Apple M1. Ordinary output was observed as 0 red pixels on M1; an unwritten Position has
unspecified rendering. Float4 alone does not establish the Position semantic. Fragment output
continues to produce the red color; malformed position declarations reject.

## Explicit point size

The authored `point` mode returns Position and a float `air.point_size` of 8.
Apple's Render Command Encoder guide and `MTLPrimitiveType.point` documentation
require `[[point_size]]`; missing point size is undefined. No default is supplied.
The renderer's optional `point` argument uses one centered point, queries and
enables `largePoints`, and expects exactly64 red pixels in its native readback.

```sh
/tmp/position-module point /tmp/position-point.spv
spirv-val --target-env vulkan1.3 /tmp/position-point.spv
VK_DRIVER_FILES=/usr/share/vulkan/icd.d/asahi_icd.json /tmp/position-render /tmp/position-point.spv /tmp/position-fragment.spv point
for mode in point-invalid-type point-invalid-stage point-duplicate point-conflict; do
  /tmp/position-module "$mode" /tmp/invalid.spv # must exit1
done
```

Baseline on Apple M1: point module validates but native output is1 red pixel,
exit1. Candidate:64 red pixels, exit0. Single and struct triangle controls remain
60000 red pixels. Invalid integer/duplicate point-size metadata was accepted before
and rejects after; invalid fragment stage is rejected explicitly rather than
reaching unrelated translation failure. This does not establish the cause of any
application point-size validation message or complete application rendering.

Contract provenance:
https://developer.apple.com/library/archive/documentation/Miscellaneous/Conceptual/MetalProgrammingGuide/Render-Ctx/Render-Ctx.html
https://developer.apple.com/documentation/metal/mtlprimitivetype/point
