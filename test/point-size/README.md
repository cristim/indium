# Explicit point-size output

All inputs are authored LLVM-C modules. Build from repository root:
```sh
c++ -std=c++17 '-DHOST_LLVM_LIBNAME="libLLVM.so"' -Iinclude -Iprivate-include test/point-size/point-module.cpp src/iridium/spirv.cpp src/iridium/dynamic-llvm.cpp -ldl -lLLVM-22 -o /tmp/point-module
/tmp/point-module point /tmp/point.spv
/tmp/point-module fragment /tmp/point-fragment.spv
spirv-val --target-env vulkan1.3 /tmp/point.spv
for mode in struct fragment ordinary; do
  /tmp/point-module "$mode" "/tmp/point-$mode.spv"
  spirv-val --target-env vulkan1.3 "/tmp/point-$mode.spv"
done
for mode in point-invalid-type point-invalid-stage point-duplicate point-conflict; do
  /tmp/point-module "$mode" /tmp/invalid-point.spv # must exit1
done
```

For native readback reuse the published PR21 authored renderer at pinned commit
`ad8c515`, with this concern's small point-mode patch. This is a test-only
dependency; no PR21 production change is required. From this repository root:
```sh
point_repo=$PWD
point_tools=$(mktemp -d /tmp/indium-point-tools.XXXXXX)
git clone --no-checkout https://github.com/cristim/indium.git "$point_tools/source"
git -C "$point_tools/source" checkout --detach ad8c515
patch -d "$point_tools/source" -p1 < "$point_repo/test/point-size/point-render.patch"
c++ -std=c++20 "$point_tools/source/test/single-position/position-render.cpp" -lvulkan -o "$point_tools/point-render"
VK_DRIVER_FILES=/usr/share/vulkan/icd.d/asahi_icd.json "$point_tools/point-render" /tmp/point.spv /tmp/point-fragment.spv point
```
Also run the renderer without `point` using /tmp/point-struct.spv and
/tmp/point-fragment.spv for the triangle control.

That test tool queries/enables largePoints, draws one centered point and reads
exact native RGBA pixels. PR21 production implementation is not required: this
fixture uses the already-supported struct Position return, not single Position.
The renderer is reused instead of including unrelated triangle implementation.
Baseline AppleM1:1redpixel/exit1 instead of requested8x8. Candidate64red/exit0.
`struct` mode with the same renderer (without point argument) remains60000red.
Valid point/struct/fragment/ordinary modules validate0. Wrong point type, stage,
duplicate and conflict modes reject1. No absent-attribute default is supplied.

Apple documents missing [[point_size]] as undefined:
https://developer.apple.com/documentation/metal/mtlprimitivetype/point
https://developer.apple.com/library/archive/documentation/Miscellaneous/Conceptual/MetalProgrammingGuide/Render-Ctx/Render-Ctx.html
This authored feature proof does not identify an application's shader fault or
claim complete application rendering. No compiled guest expression is used.
