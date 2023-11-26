//
// Created by Dan Schnurpfeil on 22.11.2023.
//

#ifndef PPR_COMPUTATIONUNIT_H
#define PPR_COMPUTATIONUNIT_H

#include "demo/vulkan_helpers.hpp"

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
  explicit ComputationUnit(const std::string &shaderPath, bool printDeviceInfo, int WORKGROUP_SIZE);
  ~ComputationUnit() noexcept;

  auto compute(vk::Buffer &out, const vk::Buffer &in, const PushParams &p) -> void;
  int WORKGROUP_SIZE;
 private:

  auto createCommandBuffer(const vk::Device &device,
                           const vk::CommandPool &cmdPool,
                           const vk::Pipeline &pipeline,
                           const vk::PipelineLayout &pipeLayout,
                           const vk::DescriptorSet &dscSet,
                           const PushParams &p
  ) -> vk::CommandBuffer;
};

#endif //PPR_COMPUTATIONUNIT_H
