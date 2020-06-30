#include <stdio.h>

#include <emscripten/emscripten.h>
#include <emscripten/html5_webgpu.h>

#include <shaders.h>

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
 * Bare minimum pipeline to draw a triangle using the shaders.
 */
static void createPipelineAndBuffers()
{
  // Shader modules
  WGPUShaderModule vertMod = createSPIRVShaderModule(device, triangle_vert.data(), triangle_vert.size() * sizeof(triangle_vert[0]));
  WGPUShaderModule fragMod = createSPIRVShaderModule(device, triangle_frag.data(), triangle_frag.size() * sizeof(triangle_frag[0]));

  // pipeline layout (used by the render pipeline, released after its creation)
  WGPUPipelineLayoutDescriptor layoutDesc = {};
  layoutDesc.bindGroupLayoutCount = 0;
  WGPUPipelineLayout pipelineLayout = wgpuDeviceCreatePipelineLayout(device, &layoutDesc);

  // begin pipeline set-up
  WGPURenderPipelineDescriptor desc = {};

  desc.layout = pipelineLayout;

  desc.vertexStage.module = vertMod;
  desc.vertexStage.entryPoint = "main";

  WGPUProgrammableStageDescriptor fragStage = {};
  fragStage.module = fragMod;
  fragStage.entryPoint = "main";
  desc.fragmentStage = &fragStage;

  desc.primitiveTopology = WGPUPrimitiveTopology_TriangleList;

  desc.sampleCount = 1;

  // describe blend
  WGPUBlendDescriptor blendDesc = {};
  blendDesc.operation = WGPUBlendOperation_Add;
  blendDesc.srcFactor = WGPUBlendFactor_SrcAlpha;
  blendDesc.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
  WGPUColorStateDescriptor colorDesc = {};
  colorDesc.format = getSwapChainFormat();
  colorDesc.alphaBlend = blendDesc;
  colorDesc.colorBlend = blendDesc;
  colorDesc.writeMask = WGPUColorWriteMask_All;

  desc.colorStateCount = 1;
  desc.colorStates = &colorDesc;

  desc.sampleMask = 0xFFFFFFFF; //<-- Note: this currently causes Emscripten to fail (sampleMask ends up as -1, which trips an assert)

  pipeline = wgpuDeviceCreateRenderPipeline(device, &desc);

  // clean-up
  wgpuPipelineLayoutRelease(pipelineLayout);

  wgpuShaderModuleRelease(fragMod);
  wgpuShaderModuleRelease(vertMod);
}

static bool frame()
{
  WGPUTextureView textureView = wgpuSwapChainGetCurrentTextureView(swapchain);
  WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(device, nullptr);

  WGPURenderPassColorAttachmentDescriptor colorDesc = {};
  colorDesc.attachment = textureView;

  WGPURenderPassDescriptor passDesc = {};
  passDesc.colorAttachmentCount = 1;
  passDesc.colorAttachments = &colorDesc;

  WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(commandEncoder, &passDesc);

  wgpuRenderPassEncoderSetPipeline(pass, pipeline);
  wgpuRenderPassEncoderDraw(pass, 3, 1, 0, 0);
  wgpuRenderPassEncoderEndPass(pass);
  wgpuRenderPassEncoderRelease(pass);

  WGPUCommandBuffer commands = wgpuCommandEncoderFinish(commandEncoder, nullptr);
  wgpuCommandEncoderRelease(commandEncoder);

  wgpuQueueSubmit(queue, 1, &commands);
  wgpuCommandBufferRelease(commands);

  wgpuTextureViewRelease(textureView);

  return true;
}

void em_draw()
{
  if (frame() == false)
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
