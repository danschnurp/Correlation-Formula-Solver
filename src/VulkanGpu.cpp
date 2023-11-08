//
// Created by Dan Schnurpfeil on 3.11.2023.
//

#include "VulkanGpu.h"

#include <vulkan/vulkan.hpp>
#include <iostream>
#include <fstream>

int VulkanGpu::prepare() {
  // Create a Vulkan instance
  vk::Instance instance = vk::createInstance(vk::InstanceCreateInfo());

  // Enumerate physical devices (GPUs)
  std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
  // Choose a physical device (GPU)

  // Print device information.
  for (const auto &i : physicalDevices) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(i, &deviceProperties);

    std::cout << "Device Name: " << deviceProperties.deviceName << std::endl;
    std::cout << "Device Type: " << deviceProperties.deviceType << std::endl;
    std::cout << "API Version: " << VK_VERSION_MAJOR(deviceProperties.apiVersion) << "."
              << VK_VERSION_MINOR(deviceProperties.apiVersion) << "." << VK_VERSION_PATCH(deviceProperties.apiVersion)
              << std::endl;
    std::cout << "Driver Version: " << deviceProperties.driverVersion << std::endl;
    std::cout << "Vendor ID: " << deviceProperties.vendorID << std::endl;
    std::cout << "Device ID: " << deviceProperties.deviceID << std::endl;

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(i, &deviceFeatures);
    std::cout << "Geometry Shader Support: " << (deviceFeatures.geometryShader ? "Yes" : "No") << std::endl;
    std::cout << "Tessellation Shader Support: " << (deviceFeatures.tessellationShader ? "Yes" : "No") << std::endl;

    std::cout << "--------------------------------------" << std::endl;
  }
    // selects last device in queue
    vk::PhysicalDevice selectedDevice = physicalDevices[physicalDevices.size() - 1];

    uint32_t queueFamilyIndex = 0;

    const float queuePrioritory = 1.0f;
    const VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
            VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            0,
            0,
            queueFamilyIndex,
            1,
            &queuePrioritory
    };

    const VkDeviceCreateInfo deviceCreateInfo = {
            VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
            0,
            0,
            1,
            &deviceQueueCreateInfo,
            0,
            0,
            0,
            0,
            0
    };


    // Create a Vulkan device
    vk::Device device = selectedDevice.createDevice(deviceCreateInfo);

  vk::PhysicalDeviceMemoryProperties memProperties = selectedDevice.getMemoryProperties();

//  std::cout << memProperties.memoryTypeCount << std::endl;

  // Create a Vulkan command pool
  vk::CommandPoolCreateInfo commandPoolCreateInfo;
  commandPoolCreateInfo.queueFamilyIndex = 0;  // Choose a suitable queue family index
  vk::CommandPool commandPool = device.createCommandPool(commandPoolCreateInfo);

  // Create a Vulkan buffer for computation
  vk::BufferCreateInfo bufferCreateInfo;
  bufferCreateInfo.size = 64 * 64;  // Size of the buffer in bytes
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

    std::ifstream file("../compute_shader.spv", std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open SPIR-V file.");
    }

    size_t fileSize = (size_t)file.tellg();
    std::vector<uint32_t> shader_buffer(fileSize / sizeof(uint32_t));

    file.seekg(0);
    file.read(reinterpret_cast<char*>(shader_buffer.data()), fileSize);

    file.close();



    // Create a shader module
    vk::ShaderModuleCreateInfo createInfo({}, shader_buffer);

    vk::ShaderModule shaderModule = device.createShaderModule(createInfo);

    // Create a pipeline layout (not shown here)
    // ...

    // Create the compute pipeline
    vk::ComputePipelineCreateInfo pipelineInfo({}, {}, pipelineLayout, nullptr, -1);
    pipelineInfo.stage.stage = vk::ShaderStageFlagBits::eCompute;
    pipelineInfo.stage.module = shaderModule;
    pipelineInfo.stage.pName = "main";

    auto pipeline_ret = device.createComputePipeline({}, pipelineInfo);

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