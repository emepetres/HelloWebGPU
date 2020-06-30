# HelloWebGPU

This a bare minimum test that shows how to draw a red triangle in the web using WebGPU and WebAssembly (Emscripten)

## Build

`build.sh [-d] [--emsdk PATH_TO_EMSDK]`

* `-d` to compile in debug mode
* `--emsdk` to specify the Emscripten SDK path

By default, the build script tries to find the Emscripten SDK at _$HOME/mono/sdks/builds/toolchains/emsdk_. This is because this work is originally meant to serve as an external test of _MONO_ + _WebGPU_ + _Emscripten_ integration.

To compile the shaders:

`glslc -Os -mfmt=num -o - -c in.vert`
`glslc -Os -mfmt=num -o - -c in.frag`

## Requirements

* [Emscripten SDK](https://github.com/emscripten-core/emsdk)
* Optional: [Shaderc](https://github.com/google/shaderc) to compile the shaders.

The sample is tested on linux, however it should work in Windows or Mac as well by translating the bash script to each platform.

## Related work

The sample is partially based on [WebGPU samples](https://github.com/austinEng/webgpu-samples) and [hello-webgpu](https://github.com/cwoffenden/hello-webgpu)
