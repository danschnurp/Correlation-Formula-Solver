//
// Created by Dan Schnurpfeil on 22.11.2023.
//

#ifndef PPR_COMPUTATIONUNIT_H
#define PPR_COMPUTATIONUNIT_H

#include "helpers/vulkan_helpers.hpp"

class ComputationUnit {
    static constexpr auto NumDescriptors = uint32_t(2);


    struct PushParams {
        uint32_t width;
        uint32_t height;
        float a;
    };

public: // data
    vk::Instance instance;

    vk::PhysicalDevice physDevice;
    vk::Device device;
    vk::ShaderModule shader;
    vk::DescriptorSetLayout dscLayout;
    mutable vk::DescriptorPool dscPool;
    vk::CommandPool cmdPool;
    vk::PipelineCache pipeCache;
    vk::PipelineLayout pipeLayout;

  vk::Pipeline pipe;
  mutable vk::CommandBuffer
      cmdBuffer;

  uint32_t compute_queue_familly_id;
 public:
  /**
   * The above function initializes a computation unit in C++ using Vulkan API, including creating an instance, device,
   * shader, descriptor set layout, command pool, pipeline cache, pipeline layout, and compute pipeline.
   *
   * @param shaderPath The `shaderPath` parameter is a string that specifies the path to the compute shader file.
   * @param printDeviceInfo A boolean flag indicating whether to print device information or not.
   * @param WORKGROUP_SIZE_param The `WORKGROUP_SIZE_param` parameter is an integer value that represents the size of the
   * workgroup for the compute shader. It is used to specify the number of threads in a workgroup when dispatching the
   * compute shader.
   */
  explicit ComputationUnit(const std::string &shaderPath, bool printDeviceInfo, int WORKGROUP_SIZE);
  ~ComputationUnit() noexcept;

  /**
   * The function `compute` performs a computation using Vulkan compute shaders and updates the output buffer with the
   * result.
   *
   * @param out A reference to a vk::Buffer object that represents the output buffer for the computation.
   * @param in The `in` parameter is a reference to a `vk::Buffer` object, which represents the input buffer for the
   * computation.
   * @param p The parameter `p` is of type `ComputationUnit::PushParams`. It contains information needed for the computation,
   * such as the width and height of the buffers.
   */
  auto compute(vk::Buffer &out, const vk::Buffer &in, const PushParams &p) -> void;
  int WORKGROUP_SIZE;
 private:

  auto createCommandBuffer(const vk::Device &device,
                           const vk::CommandPool &cmdPool,
                           const vk::Pipeline &pipeline,
                           const vk::PipelineLayout &pipeLayout,
                           const vk::DescriptorSet &dscSet,
                           const PushParams &p
  ) const -> vk::CommandBuffer;
};

#endif //PPR_COMPUTATIONUNIT_H
