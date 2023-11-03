#include "VulkanGpu.h"

#include <vulkan/vulkan.hpp>
#include <iostream>

int VulkanGpu::prepare() {
  // Create a Vulkan instance
  vk::Instance instance = vk::createInstance(vk::InstanceCreateInfo());

  // Enumerate physical devices (GPUs)
  std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
  std::cout << physicalDevices[1] << std::endl;
  // Choose a physical device (GPU)
  vk::PhysicalDevice selectedDevice = physicalDevices[0];



  // Create a Vulkan device
  vk::Device device = selectedDevice.createDevice(vk::DeviceCreateInfo());

  vk::PhysicalDeviceMemoryProperties memProperties = selectedDevice.getMemoryProperties();

  std::cout << memProperties.memoryTypeCount << std::endl;

  // Create a Vulkan command pool
  vk::CommandPoolCreateInfo commandPoolCreateInfo;
  commandPoolCreateInfo.queueFamilyIndex = 0;  // Choose a suitable queue family index
  vk::CommandPool commandPool = device.createCommandPool(commandPoolCreateInfo);

  // Create a Vulkan buffer for computation
  vk::BufferCreateInfo bufferCreateInfo;
  bufferCreateInfo.size = 256 * 256;  // Size of the buffer in bytes
  bufferCreateInfo.usage = vk::BufferUsageFlagBits::eStorageBuffer;
  vk::Buffer buffer = device.createBuffer(bufferCreateInfo);

  // Allocate memory for the buffer
  vk::MemoryRequirements memRequirements = device.getBufferMemoryRequirements(buffer);
  vk::MemoryAllocateInfo allocInfo;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = 0;  // Choose a suitable memory type index
  vk::DeviceMemory bufferMemory = device.allocateMemory(allocInfo);

  // Bind the buffer to the allocated memory
  device.bindBufferMemory(buffer, bufferMemory, 0);

  // Create a Vulkan descriptor set layout
  vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
  vk::DescriptorSetLayout descriptorSetLayout = device.createDescriptorSetLayout(descriptorSetLayoutCreateInfo);

  // Create a Vulkan pipeline layout
  vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
  pipelineLayoutCreateInfo.setLayoutCount = 1;
  pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
  vk::PipelineLayout pipelineLayout = device.createPipelineLayout(pipelineLayoutCreateInfo);

  // Create a Vulkan descriptor pool
  vk::DescriptorPoolSize poolSize(vk::DescriptorType::eStorageBuffer, 1);
  vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo;
  descriptorPoolCreateInfo.maxSets = 1;
  descriptorPoolCreateInfo.poolSizeCount = 1;
  descriptorPoolCreateInfo.pPoolSizes = &poolSize;
  vk::DescriptorPool descriptorPool = device.createDescriptorPool(descriptorPoolCreateInfo);

  // Create a Vulkan descriptor set
  vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo;
  descriptorSetAllocateInfo.descriptorPool = descriptorPool;
  descriptorSetAllocateInfo.descriptorSetCount = 1;
  descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;
  std::vector<vk::DescriptorSet> descriptorSets = device.allocateDescriptorSets(descriptorSetAllocateInfo);

  // Create a Vulkan compute pipeline
  vk::PipelineShaderStageCreateInfo shaderStageCreateInfo;
  shaderStageCreateInfo.stage = vk::ShaderStageFlagBits::eCompute;

  const char *computeShaderCode = R"(
#version 450

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(set = 0, binding = 0) buffer inputBuffer {
    uint data[];
};

layout(set = 0, binding = 1) buffer outputBuffer {
    uint result[];
};

void main() {
    uint idx = gl_GlobalInvocationID.x;
    result[idx] = data[idx] * 2;
}
)";

  shaderStageCreateInfo.module = (VkShaderModule) computeShaderCode /* todo Load your compute shader module */;
  shaderStageCreateInfo.pName = "main";  // Entry point in your compute shader
  vk::ComputePipelineCreateInfo pipelineCreateInfo;
  pipelineCreateInfo.stage = shaderStageCreateInfo;
  auto pipeline_ret = device.createComputePipeline(vk::PipelineCache(), pipelineCreateInfo);
  std::cout << pipeline_ret.result << std::endl;
  vk::Pipeline pipeline = pipeline_ret.value;
  // Create a Vulkan command buffer
  vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
  commandBufferAllocateInfo.commandPool = commandPool;
  commandBufferAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
  commandBufferAllocateInfo.commandBufferCount = 1;
  std::vector<vk::CommandBuffer> commandBuffers = device.allocateCommandBuffers(commandBufferAllocateInfo);

  vk::CommandBuffer commandBuffer = commandBuffers[0];

  // Record a command to execute the compute shader
  commandBuffer.begin(vk::CommandBufferBeginInfo());
  commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, pipeline);
  commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, pipelineLayout, 0, descriptorSets, {});
  commandBuffer.dispatch(1, 1, 1);  // Set the number of work groups
  commandBuffer.end();

  // Submit the command buffer for execution
  vk::Queue queue = device.getQueue(0, 0);  // Choose a suitable queue
  vk::SubmitInfo submitInfo;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &commandBuffer;
  queue.submit(submitInfo, vk::Fence());
  queue.waitIdle();

  // Clean up Vulkan resources
  device.freeDescriptorSets(descriptorPool, descriptorSets);
  device.destroyDescriptorPool(descriptorPool);
  device.destroyPipeline(pipeline);
  device.destroyPipelineLayout(pipelineLayout);
  device.destroyDescriptorSetLayout(descriptorSetLayout);
  device.freeMemory(bufferMemory);
  device.destroyBuffer(buffer);
  device.destroyCommandPool(commandPool);
  device.destroy();
  instance.destroy();

  return 0;
}