//
// Created by Dan Schnurpfeil on 22.11.2023.
//

#include "ComputationUnit.h"


#include <vulkan/vulkan.hpp>

#define ARR_VIEW(x) uint32_t(x.size()), x.data()
#define ST_VIEW(s)  uint32_t(sizeof(s)), &s

using namespace vuh;
namespace {
    constexpr uint32_t WORKGROUP_SIZE = 16; ///< compute shader workgroup dimension is WORKGROUP_SIZE x WORKGROUP_SIZE

#ifdef NDEBUG
    constexpr bool enableValidation = false;
#else
    constexpr bool enableValidation = false;
#endif
} // namespace


/// Constructor
ComputationUnit::ComputationUnit(const std::string &shaderPath, bool printDeviceInfo) {
  auto appInfo = vk::ApplicationInfo("PPR Compute app",
                                     0,
                                     "no_engine",
                                     0,
                                     VK_API_VERSION_1_0); // The only important field here is apiVersion
  auto createInfo = vk::InstanceCreateInfo(vk::InstanceCreateFlags(), &appInfo, {}, {});
  instance = vk::createInstance(createInfo);
  if (printDeviceInfo) {
    // Print device information.
    physDevice = instance.enumeratePhysicalDevices()[0]; // just use the first device
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(physDevice, &deviceProperties);

    std::cout << "Device Name: " << deviceProperties.deviceName << std::endl;
    std::cout << "Device Type: " << deviceProperties.deviceType << std::endl;
    std::cout << "API Version: " << VK_VERSION_MAJOR(deviceProperties.apiVersion) << "."
              << VK_VERSION_MINOR(deviceProperties.apiVersion) << "." << VK_VERSION_PATCH(deviceProperties.apiVersion)
              << std::endl;
    std::cout << "Driver Version: " << deviceProperties.driverVersion << std::endl;
    std::cout << "Vendor ID: " << deviceProperties.vendorID << std::endl;
    std::cout << "Device ID: " << deviceProperties.deviceID << std::endl;

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(physDevice, &deviceFeatures);
    std::cout << "Geometry Shader Support: " << (deviceFeatures.geometryShader ? "Yes" : "No") << std::endl;
    std::cout << "Tessellation Shader Support: " << (deviceFeatures.tessellationShader ? "Yes" : "No") << std::endl;

    std::cout << "--------------------------------------" << std::endl;
  }

  compute_queue_familly_id = getComputeQueueFamilyId(physDevice);
  device = createDevice(physDevice, {}, compute_queue_familly_id);
  shader = loadShader(device, shaderPath.c_str());
  auto bindLayout = std::array<vk::DescriptorSetLayoutBinding, NumDescriptors>{{
                                                                                   {0,
                                                                                    vk::DescriptorType::eStorageBuffer,
                                                                                    1,
                                                                                    vk::ShaderStageFlagBits::eCompute},
                                                                                   {1,
                                                                                          vk::DescriptorType::eStorageBuffer,
                                                                                          1,
                                                                                          vk::ShaderStageFlagBits::eCompute}
                                                                                 }};
    auto layoutCI = vk::DescriptorSetLayoutCreateInfo(vk::DescriptorSetLayoutCreateFlags(), ARR_VIEW(bindLayout));
    dscLayout = device.createDescriptorSetLayout(layoutCI);

    auto descriptorPoolSize = vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, NumDescriptors);
    auto descriptorPoolCI = vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlags(), 1, 1, &descriptorPoolSize);
    dscPool = device.createDescriptorPool(descriptorPoolCI);

    auto commandPoolCI = vk::CommandPoolCreateInfo(vk::CommandPoolCreateFlags(), compute_queue_familly_id);
    cmdPool = device.createCommandPool(commandPoolCI);
    pipeCache = device.createPipelineCache(vk::PipelineCacheCreateInfo());

    auto pushConstantsRange = vk::PushConstantRange(vk::ShaderStageFlagBits::eCompute, 0, sizeof(PushParams));
    auto pipelineLayoutCI =
            vk::PipelineLayoutCreateInfo(vk::PipelineLayoutCreateFlags(), 1, &dscLayout, 1, &pushConstantsRange);
    pipeLayout = device.createPipelineLayout(pipelineLayoutCI);

    // specialize constants of the shader
    auto specEntries = std::array<vk::SpecializationMapEntry, 2>{
            {{0, 0, sizeof(int)}, {1, 1 * sizeof(int), sizeof(int)}}
    };
    auto specValues = std::array<int, 2>{WORKGROUP_SIZE, WORKGROUP_SIZE};
    auto specInfo = vk::SpecializationInfo(ARR_VIEW(specEntries), specValues.size() * sizeof(int), specValues.data());

    // Specify the compute shader stage, and it's entry point (main), and specializations
    auto stageCI = vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(),
                                                     vk::ShaderStageFlagBits::eCompute,
                                                     shader,
                                                     "main",
                                                     &specInfo);
    auto pipelineCI = vk::ComputePipelineCreateInfo(vk::PipelineCreateFlags(), stageCI, pipeLayout);
    pipe = device.createComputePipeline(pipeCache, pipelineCI, nullptr).value;
    cmdBuffer = vk::CommandBuffer{};
}

/// Destructor
ComputationUnit::~ComputationUnit() noexcept {
    device.destroyPipeline(pipe);
    device.destroyPipelineLayout(pipeLayout);
    device.destroyPipelineCache(pipeCache);
    device.destroyCommandPool(cmdPool);
    device.destroyDescriptorPool(dscPool);
    device.destroyDescriptorSetLayout(dscLayout);
    device.destroyShaderModule(shader);
    device.destroy();

    instance.destroy();
}

///
auto ComputationUnit::compute(vk::Buffer &out, const vk::Buffer &in, const ComputationUnit::PushParams &p
) const -> void {

    auto descriptorSetAI = vk::DescriptorSetAllocateInfo(dscPool, 1, &dscLayout);
    auto dscSet = device.allocateDescriptorSets(descriptorSetAI)[0];

    auto outInfo = vk::DescriptorBufferInfo(out, 0, sizeof(float) * (p.width * p.height));
    auto inInfo = vk::DescriptorBufferInfo(in, 0, sizeof(float) * (p.width * p.height));

    auto writeDsSets = std::array<vk::WriteDescriptorSet, NumDescriptors>{{
                                                                                  {dscSet, 0, 0, 1,
                                                                                   vk::DescriptorType::eStorageBuffer,
                                                                                   nullptr, &outInfo},
                                                                                  {dscSet, 1, 0, 1,
                                                                                   vk::DescriptorType::eStorageBuffer,
                                                                                   nullptr, &inInfo}
                                                                          }};
    device.updateDescriptorSets(writeDsSets, {});
    cmdBuffer = createCommandBuffer(device, cmdPool, pipe, pipeLayout, dscSet, p);

    assert(cmdBuffer != vk::CommandBuffer{}); // TODO: this should be a check for a valid command buffer
    auto submitInfo = vk::SubmitInfo(0, nullptr, nullptr, 1, &cmdBuffer); // submit a single command buffer

    // submit the command buffer to the queue and set up a fence.
    auto queue = device.getQueue(compute_queue_familly_id,
                                 0); // 0 is the queue index in the family, by default just the first one is used
    auto fence =
            device.createFence(vk::FenceCreateInfo()); // fence makes sure the control is not returned to CPU till command buffer is depleted
    queue.submit({submitInfo}, fence);
    device.waitForFences({fence}, true, uint64_t(-1));       // wait for the fence indefinitely
    device.destroyFence(fence);

    device.destroyDescriptorPool(dscPool);
    device.resetCommandPool(cmdPool, vk::CommandPoolResetFlags());
    auto descriptorPoolSize = vk::DescriptorPoolSize(vk::DescriptorType::eStorageBuffer, NumDescriptors);
    auto descriptorPoolCI = vk::DescriptorPoolCreateInfo(vk::DescriptorPoolCreateFlags(), 1, 1, &descriptorPoolSize);
    dscPool = device.createDescriptorPool(descriptorPoolCI);

}

/// Create command buffer, push the push constants, bind descriptors and define the work batch size.
/// All command buffers allocated from given command pool must be submitted to queues of corresponding
/// family ONLY.
auto ComputationUnit::createCommandBuffer(const vk::Device &device,
                                                const vk::CommandPool &cmdPool,
                                                const vk::Pipeline &pipeline,
                                                const vk::PipelineLayout &pipeLayout,
                                                const vk::DescriptorSet &dscSet,
                                                const ComputationUnit::PushParams &p
) -> vk::CommandBuffer {
    // allocate a command buffer from the command pool.
    auto commandBufferAI = vk::CommandBufferAllocateInfo(cmdPool, vk::CommandBufferLevel::ePrimary, 1);
    auto commandBuffer = device.allocateCommandBuffers(commandBufferAI)[0];

    // Start recording commands into the newly allocated command buffer.
//	auto beginInfo = vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit); // buffer is only submitted and used once
    auto beginInfo = vk::CommandBufferBeginInfo();
    commandBuffer.begin(beginInfo);

    // Before dispatch bind a pipeline, AND a descriptor set.
    // The validation layer will NOT give warnings if you forget those.
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipeLayout, 0, {dscSet}, {});

    commandBuffer.pushConstants(pipeLayout, vk::ShaderStageFlagBits::eCompute, 0, ST_VIEW(p));

    // Start the compute pipeline, and execute the compute shader.
    // The number of workgroups is specified in the arguments.
    commandBuffer.dispatch(div_up(p.width, WORKGROUP_SIZE), div_up(p.height, WORKGROUP_SIZE), 1);
    commandBuffer.end(); // end recording commands
    return commandBuffer;
}