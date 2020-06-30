#include <stdio.h>

#include <emscripten/emscripten.h>
#include <emscripten/html5_webgpu.h>

WGPUDevice device;
WGPUQueue queue;
WGPUSwapChain swapchain;

WGPURenderPipeline pipeline;
WGPUBuffer vertBuf; // vertex buffer with triangle position and colours
WGPUBuffer indxBuf; // index buffer
WGPUBuffer uRotBuf; // uniform buffer (containing the rotation angle)
WGPUBindGroup bindGroup;

static WGPUTextureFormat getSwapChainFormat()
{
  return WGPUTextureFormat_BGRA8Unorm;
}

static WGPUSwapChain createSwapChain(WGPUDevice device)
{
  WGPUSurfaceDescriptorFromHTMLCanvasId canvDesc = {};
  canvDesc.chain.sType = WGPUSType_SurfaceDescriptorFromHTMLCanvasId;
  canvDesc.id = "canvas";

  WGPUSurfaceDescriptor surfDesc = {};
  surfDesc.nextInChain = reinterpret_cast<WGPUChainedStruct *>(&canvDesc);

  WGPUSurface surface = wgpuInstanceCreateSurface(nullptr, &surfDesc);

  WGPUSwapChainDescriptor swapDesc = {};
  swapDesc.usage = WGPUTextureUsage_OutputAttachment;
  swapDesc.format = getSwapChainFormat();
  swapDesc.width = 800;
  swapDesc.height = 450;
  swapDesc.presentMode = WGPUPresentMode_Fifo;

  WGPUSwapChain swapchain = wgpuDeviceCreateSwapChain(device, surface, &swapDesc);

  return swapchain;
}

/**
 * Bare minimum pipeline to draw a triangle using the above shaders.
 */
static void createPipelineAndBuffers()
{
}

static bool redraw()
{
  printf("hello, world!\n");
  return true;
}

void em_draw()
{
  if (redraw() == false)
  {
    emscripten_cancel_main_loop();
  }
}

extern "C" int __main__(int /*argc*/, char * /*argv*/[])
{
  if ((device = emscripten_webgpu_get_device()))
  {
    queue = wgpuDeviceGetDefaultQueue(device);
    swapchain = createSwapChain(device);
    createPipelineAndBuffers();

    emscripten_set_main_loop(em_draw, 0, false);
  }
  return 0;
}
