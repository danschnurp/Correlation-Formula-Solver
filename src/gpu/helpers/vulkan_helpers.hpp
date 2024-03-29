#pragma once



#include <vulkan/vulkan.hpp>

#include <iostream>
#include <fstream>
#include <type_traits>
#include <memory>


#include <vulkan/vulkan.hpp>

#include <vector>

namespace vuh {

    inline auto div_up(uint32_t x, uint32_t y) { return (x + y - 1u) / y; }

    auto readShaderSrc(const char *filename) -> std::vector<char>;

    auto loadShader(const vk::Device &device,
                    const char *filename,
                    vk::ShaderModuleCreateFlags flags = vk::ShaderModuleCreateFlags()
    ) -> vk::ShaderModule;

    auto getComputeQueueFamilyId(const vk::PhysicalDevice &physicalDevice) -> uint32_t;

    auto createDevice(const vk::PhysicalDevice &physicalDevice,
                      const std::vector<const char *> &layers,
                      uint32_t queueFamilyID) -> vk::Device;

    auto createBuffer(const vk::Device &device,
                      uint32_t bufSize,
                      vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eStorageBuffer
    ) -> vk::Buffer;

    auto selectMemory(const vk::PhysicalDevice &physDev,
                      const vk::Device &device,
                      const vk::Buffer &buf,
                      const vk::MemoryPropertyFlags properties ///< desired memory properties
    ) -> uint32_t;

    auto allocMemory(const vk::PhysicalDevice &physDev, const vk::Device &device, const vk::Buffer &buf, uint32_t memory_id
    ) -> vk::DeviceMemory;

    auto copyBuf(const vk::Buffer &src,
                 vk::Buffer &dst,
                 const uint32_t size,
                 const vk::Device &device,
                 const vk::PhysicalDevice &physDev) -> void;

/// Device buffer owning its chunk of memory.
template<class T>
class Array {
  // Helper class to access to (host-visible!!!) device memory from the host.
  // Unmapping memory is not necessary.
  struct BufferHostView {
    using ptr_type = T *;

    const vk::Device device;
    const vk::DeviceMemory devMemory;
    const ptr_type data; ///< points to the first element
    const size_t size;   ///< number of elements

    /// Constructor
    explicit BufferHostView(vk::Device device, vk::DeviceMemory devMem, size_t nelements ///< number of elements
    )
        : device(device),
          devMemory(devMem),
          data(ptr_type(device.mapMemory(devMem, 0, nelements * sizeof(T)))),
          size(nelements) {}

    auto begin() -> ptr_type { return data; }
    auto end() -> ptr_type { return data + size; }
  }; // BufferHostView

 private:
  vk::Buffer _buf;                        ///< device buffer
  vk::DeviceMemory _mem;                  ///< associated chunk of device memorys
  vk::PhysicalDevice _physdev;            ///< physical device owning the memory
  std::unique_ptr<const vk::Device>
      _dev; ///< pointer to logical device. no real ownership, just to provide value semantics to the class.
  vk::MemoryPropertyFlags _flags;         ///< Actual flags of allocated memory. Can be a superset of requested flags.
  size_t
      _size;                           ///< number of elements. actual allocated memory may be a bit bigger than necessary.
 public:
  using value_type = T;

  Array(Array &&) = default;
  auto operator=(Array &&) -> Array & = default;

  /// Constructor
  explicit Array(const vk::Device &device,
                 const vk::PhysicalDevice &physDevice,
                 uint32_t n_elements ///< number of elements of corresponding type
      ,
                 vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eDeviceLocal,
                 vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eStorageBuffer
  )
      : Array(device,
              physDevice,
              createBuffer(device, n_elements * sizeof(T), update_usage(physDevice, properties, usage)),
              properties,
              n_elements) {}

  /// Destructor
  ~Array() noexcept {
    if (_dev) {
      _dev->freeMemory(_mem);
      _dev->destroyBuffer(_buf);
      _dev.release();
    }
  }

  template<class C>
  static auto fromHost(C &&c,
                       const vk::Device &device,
                       const vk::PhysicalDevice &physDev,
                       vk::MemoryPropertyFlags properties = vk::MemoryPropertyFlagBits::eDeviceLocal,
                       vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eStorageBuffer
  ) -> Array {
    auto r = Array<T>(device, physDev, uint32_t(c.size()), properties, usage);
    if (r._flags & vk::MemoryPropertyFlagBits::eHostVisible) { // memory is host-visible
      std::copy(begin(c), end(c), r.host_view().data);
    } else { // memory is not host visible, use staging buffer
      auto stage_buf = fromHost(std::forward<C>(c),
                                device,
                                physDev,
                                vk::MemoryPropertyFlagBits::eHostVisible,
                                vk::BufferUsageFlagBits::eTransferSrc);
      copyBuf(stage_buf, r, stage_buf.size() * sizeof(T), device, physDev);
    }
    return r;
  }

  operator vk::Buffer &() { return *reinterpret_cast<vk::Buffer *>(this + offsetof(Array, _buf)); }
  operator const vk::Buffer &() const { return *reinterpret_cast<const vk::Buffer *>(this + offsetof(Array, _buf)); }

  /// @return number of items in the buffer
  auto size() const -> size_t {
    return _size;
  }

  template<class C>
  auto to_host(C &c) -> void {
    if (_flags & vk::MemoryPropertyFlagBits::eHostVisible) { // memory IS host visible
      auto hv = host_view();
      c.resize(size());
      std::copy(std::begin(hv), std::end(hv), c.data());
    } else { // memory is not host visible, use staging buffer
      // copy device memory to staging buffer
      auto stage_buf = Array(*_dev,
                             _physdev,
                             size(),
                             vk::MemoryPropertyFlagBits::eHostVisible,
                             vk::BufferUsageFlagBits::eTransferDst);
      copyBuf(_buf, stage_buf, size() * sizeof(T), *_dev, _physdev);
      stage_buf.to_host(c); // copy from staging buffer to host
    }
  }

 private: // helpers
  ///
  auto host_view() -> BufferHostView { return BufferHostView(*_dev, _mem, size()); }

  /// Helper constructor
  explicit Array(const vk::Device &device,
                 const vk::PhysicalDevice &physDevice,
                 vk::Buffer buffer,
                 vk::MemoryPropertyFlags properties,
                 size_t size
  )
      : Array(device, physDevice, buffer, size, selectMemory(physDevice, device, buffer, properties)) {}

  /// Helper constructor. This one does the actual construction and binding.
  explicit Array(const vk::Device &device,
                 const vk::PhysicalDevice &physDevice,
                 vk::Buffer buf,
                 size_t size,
                 uint32_t memory_id)
      : _buf(buf),
        _mem(allocMemory(physDevice, device, buf, memory_id)),
        _physdev(physDevice),
        _dev(&device),
        _flags(physDevice.getMemoryProperties().memoryTypes[memory_id].propertyFlags),
        _size(size) {
    device.bindBufferMemory(buf, _mem, 0);
  }

  /// crutch to modify buffer usage
  auto update_usage(const vk::PhysicalDevice &physDevice, vk::MemoryPropertyFlags properties, vk::BufferUsageFlags usage
  ) -> vk::BufferUsageFlags {
    if (physDevice.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu
        && properties == vk::MemoryPropertyFlagBits::eDeviceLocal
        && usage == vk::BufferUsageFlagBits::eStorageBuffer) {
      usage |= vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst;
    }
    return usage;
  }
}; // Array

} // namespace vuh
