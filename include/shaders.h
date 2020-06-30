#include <stdint.h>
#include <vector>

#include <webgpu/webgpu.h>

/**
 * Helper to create a shader from SPIR-V IR.
 *
 * \param[in] code shader source (output using the \c -V \c -x options in \c glslangValidator)
 * \param[in] size size of \a code in bytes
 * \param[in] label optional shader name
 */
WGPUShaderModule createSPIRVShaderModule(WGPUDevice device, const uint32_t *code, uint32_t size, const char *label = nullptr);

extern std::vector<uint32_t> const triangle_vert;
extern std::vector<uint32_t> const triangle_frag;
