//
// Created by Dan Schnurpfeil on 3.11.2023.
//

#include "VulkanGpu.h"

#include <vulkan/vulkan.hpp>
#include <iostream>
#include <fstream>


VkPipelineLayout createPipelineLayout(VkDevice device) {
    // Descriptor Set Layouts
    VkDescriptorSetLayoutBinding xBufferLayoutBinding = {};
    xBufferLayoutBinding.binding = 0;
    xBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    xBufferLayoutBinding.descriptorCount = 1;
    xBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding yBufferLayoutBinding = {};
    yBufferLayoutBinding.binding = 1;
    yBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    yBufferLayoutBinding.descriptorCount = 1;
    yBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding resultBufferLayoutBinding = {};
    resultBufferLayoutBinding.binding = 2;
    resultBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    resultBufferLayoutBinding.descriptorCount = 1;
    resultBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding bindings[] = {xBufferLayoutBinding, yBufferLayoutBinding, resultBufferLayoutBinding};

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 3;
    layoutInfo.pBindings = bindings;

    VkDescriptorSetLayout descriptorSetLayout;
    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        // Handle creation failure
    }

    // Pipeline Layout
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

    VkPipelineLayout pipelineLayout;
    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        // Handle creation failure
    }

    return pipelineLayout;
}

vk::Device create_device(vk::PhysicalDevice selectedDevice) {
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
    return selectedDevice.createDevice(deviceCreateInfo);
}

vk::PhysicalDevice create_physical_device(vk::Instance &instance) {
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

    vk::PhysicalDeviceMemoryProperties memProperties = selectedDevice.getMemoryProperties();
//    std::cout << memProperties.memoryTypes << std::endl;
    return selectedDevice;

}

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    // Handle failure to find a suitable memory type
    throw std::runtime_error("Failed to find suitable memory type");
}

// Function to create a Vulkan buffer
VkBuffer createBuffer(VkDevice device, vk::PhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceMemory& bufferMemory) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer buffer;
    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
        // Handle buffer creation failure
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, physicalDevice);

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
        // Handle memory allocation failure
    }

    vkBindBufferMemory(device, buffer, bufferMemory, 0);

    return buffer;
}

// Function to create a Vulkan descriptor pool
VkDescriptorPool createDescriptorPool(VkDevice device, uint32_t maxSets) {
    VkDescriptorPoolSize poolSizes[3];

    // Specify the number of descriptors for each type
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[0].descriptorCount = maxSets; // Assuming each set has one storage buffer

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[1].descriptorCount = maxSets;

    poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[2].descriptorCount = maxSets;

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 3; // Number of elements in the poolSizes array
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = maxSets;

    VkDescriptorPool descriptorPool;
    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        // Handle descriptor pool creation failure
    }

    return descriptorPool;
}


// Function to create Vulkan descriptor set
VkDescriptorSet createDescriptorSet(VkDevice device, int max_set, VkDescriptorSetLayout descriptorSetLayout, VkBuffer xBuffer, VkBuffer yBuffer, VkBuffer resultBuffer) {
    // Allocate descriptor sets
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = createDescriptorPool(device, max_set);
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &descriptorSetLayout;

    VkDescriptorSet descriptorSet;
    if (vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet) != VK_SUCCESS) {
        // Handle descriptor set allocation failure
    }

    // Update descriptor sets with buffer information
    VkDescriptorBufferInfo xBufferInfo = {};
    xBufferInfo.buffer = xBuffer;
    xBufferInfo.offset = 0;
    xBufferInfo.range = VK_WHOLE_SIZE;

    VkDescriptorBufferInfo yBufferInfo = {};
    yBufferInfo.buffer = yBuffer;
    yBufferInfo.offset = 0;
    yBufferInfo.range = VK_WHOLE_SIZE;

    VkDescriptorBufferInfo resultBufferInfo = {};
    resultBufferInfo.buffer = resultBuffer;
    resultBufferInfo.offset = 0;
    resultBufferInfo.range = VK_WHOLE_SIZE;

    VkWriteDescriptorSet descriptorWrites[] = {
            { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, descriptorSet, 0, 0, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, nullptr, &xBufferInfo, nullptr },
            { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, descriptorSet, 1, 0, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, nullptr, &yBufferInfo, nullptr },
            { VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET, nullptr, descriptorSet, 2, 0, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, nullptr, &resultBufferInfo, nullptr }
    };

    vkUpdateDescriptorSets(device, 3, descriptorWrites, 0, nullptr);

    return descriptorSet;
}

std::vector<uint32_t> loadShaderCode(const char* filePath) {
    // Open the file
    std::ifstream file(filePath, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        // Handle file opening failure
        throw std::runtime_error("Failed to open shader file");
    }

    // Determine the file size
    size_t fileSize = static_cast<size_t>(file.tellg());
    file.seekg(0);

    // Read the file into a buffer
    std::vector<char> buffer(fileSize);
    file.read(buffer.data(), fileSize);

    // Close the file
    file.close();

    // Convert the char buffer to uint32_t buffer (assuming the shader code is in SPIR-V format)
    std::vector<uint32_t> shaderCode(buffer.size() / sizeof(uint32_t));
    memcpy(shaderCode.data(), buffer.data(), buffer.size());

    return shaderCode;
}

// Function to create a Vulkan compute pipeline
VkPipeline createComputePipeline(VkDevice device, VkPipelineLayout pipelineLayout, const char* computeShaderPath) {
    // Load the compute shader
    std::vector<uint32_t> computeShaderCode = loadShaderCode(computeShaderPath); // Implement loadShaderCode function

    // Create the compute shader module
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = computeShaderCode.size() * sizeof(uint32_t);
    createInfo.pCode = computeShaderCode.data();

    VkShaderModule computeShaderModule;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &computeShaderModule) != VK_SUCCESS) {
        // Handle shader module creation failure
    }

    // Create compute pipeline
    VkPipelineShaderStageCreateInfo computeShaderStageInfo = {};
    computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    computeShaderStageInfo.module = computeShaderModule;
    computeShaderStageInfo.pName = "main"; // Entry point name in the shader

    VkComputePipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.stage = computeShaderStageInfo;
    pipelineInfo.layout = pipelineLayout;

    VkPipeline pipeline;
    if (vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline) != VK_SUCCESS) {
        // Handle pipeline creation failure
    }

    // Clean up shader module
    vkDestroyShaderModule(device, computeShaderModule, nullptr);

    return pipeline;
}

// Function to create a Vulkan descriptor set layout
VkDescriptorSetLayout createDescriptorSetLayout(VkDevice device) {
    // Descriptor Set Layout Bindings
    VkDescriptorSetLayoutBinding xBufferLayoutBinding = {};
    xBufferLayoutBinding.binding = 0;
    xBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    xBufferLayoutBinding.descriptorCount = 1;
    xBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding yBufferLayoutBinding = {};
    yBufferLayoutBinding.binding = 1;
    yBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    yBufferLayoutBinding.descriptorCount = 1;
    yBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding resultBufferLayoutBinding = {};
    resultBufferLayoutBinding.binding = 2;
    resultBufferLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    resultBufferLayoutBinding.descriptorCount = 1;
    resultBufferLayoutBinding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutBinding bindings[] = {xBufferLayoutBinding, yBufferLayoutBinding, resultBufferLayoutBinding};

    // Descriptor Set Layout
    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 3;
    layoutInfo.pBindings = bindings;

    VkDescriptorSetLayout descriptorSetLayout;
    if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        // Handle descriptor set layout creation failure
    }

    return descriptorSetLayout;
}


// Function to create a Vulkan command pool
VkCommandPool createCommandPool(VkDevice device, uint32_t queueFamilyIndex) {
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndex;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT; // Optional, depending on your use case

    VkCommandPool commandPool;
    if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
        // Handle command pool creation failure
    }

    return commandPool;
}

// Function to create a Vulkan command buffer
VkCommandBuffer createCommandBuffer(VkDevice device, VkCommandPool commandPool) {
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; // Can be VK_COMMAND_BUFFER_LEVEL_SECONDARY for secondary command buffers
    allocInfo.commandBufferCount = 1; // Number of command buffers to allocate

    VkCommandBuffer commandBuffer;
    if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
        // Handle command buffer allocation failure
    }

    return commandBuffer;
}

// Function to submit a command buffer to a Vulkan queue
void submitCommandBuffer(VkQueue queue, VkCommandBuffer commandBuffer) {
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue); // Wait for the queue to become idle (optional)
}

// Function to create a Vulkan queue
VkQueue createQueue(VkDevice device, uint32_t queueFamilyIndex) {
    VkQueue queue;
    vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);
    return queue;
}

float VulkanGpu::compute_correlation(const std::vector<float>& dataArray_x, const std::vector<float>& dataArray_y) {

    // Create a Vulkan instance
    vk::Instance instance = vk::createInstance(vk::InstanceCreateInfo());
    auto p_device = create_physical_device(instance);
    vk::Device device = create_device(p_device);



    VkDeviceMemory xBufferMemory, yBufferMemory, resultBufferMemory;
    VkBuffer xBuffer = createBuffer(device, p_device, dataArray_x.size() * sizeof(float), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, xBufferMemory);
    VkBuffer yBuffer = createBuffer(device, p_device, dataArray_y.size() * sizeof(float), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, yBufferMemory);
    VkBuffer resultBuffer = createBuffer(device, p_device, sizeof(float), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, resultBufferMemory);

    VkDescriptorSetLayout descriptorSetLayout = createDescriptorSetLayout(device);
    VkDescriptorSet descriptorSet = createDescriptorSet(device, dataArray_x.size(), descriptorSetLayout, xBuffer, yBuffer, resultBuffer);

    VkPipelineLayout pipelineLayout = createPipelineLayout(device);
    VkPipeline computePipeline = createComputePipeline(device, pipelineLayout, "../compute_shader.spv");

    // Assuming you have a Vulkan device and a queue family index
    VkCommandPool commandPool = createCommandPool(device, 0);
    VkCommandBuffer commandBuffer = createCommandBuffer(device, commandPool);


    uint32_t numGroupsX = dataArray_x.size();
    uint32_t numGroupsY = 1;
    uint32_t numGroupsZ = 1;



    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // Depending on your use case
    beginInfo.pInheritanceInfo = nullptr; // Optional
    VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("vkBeginCommandBuffer failed");
    }


// Dispatch compute shader
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
    vkCmdDispatch(commandBuffer, numGroupsX, numGroupsY, numGroupsZ);


    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
    VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_HOST_BIT;

    VkMemoryBarrier memoryBarrier = {};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    memoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    memoryBarrier.dstAccessMask = VK_ACCESS_HOST_READ_BIT;

    VkQueue queue = createQueue(device, 0);
    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
//    vkQueueWaitIdle(queue); // Optional: Wait for the queue to become idle

    vkCmdPipelineBarrier(commandBuffer, srcStageMask, dstStageMask, 0, 1, &memoryBarrier, 0, nullptr, 0, nullptr);

    // Map the result buffer and read the correlation coefficient
    float* mappedData;
    vkMapMemory(device, resultBufferMemory, 0, sizeof(float), 0, (void**)&mappedData);
    float correlation = *mappedData;
    vkUnmapMemory(device, resultBufferMemory);

    vkEndCommandBuffer(commandBuffer);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
    vkDestroyCommandPool(device, commandPool, nullptr);

// Don't forget to destroy the resources when they are no longer needed
    vkDestroyBuffer(device, xBuffer, nullptr);
    vkDestroyBuffer(device, yBuffer, nullptr);
    vkDestroyBuffer(device, resultBuffer, nullptr);

    vkFreeMemory(device, xBufferMemory, nullptr);
    vkFreeMemory(device, yBufferMemory, nullptr);
    vkFreeMemory(device, resultBufferMemory, nullptr);

    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
//    vkDestroyDescriptorSet(device, descriptorSet, nullptr);

    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyPipeline(device, computePipeline, nullptr);

  return correlation;
}