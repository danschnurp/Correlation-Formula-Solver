# Discovering the Correlation Formula between 3D Accelerometer Data and Heart Rate

### Prerequisites

- Download the relevant portions of data files - HR and ACC files from
  https://physionet.org/content/big-ideas-glycemic-wearable/1.1.2/ (HR and ACC)

- [VulkanSDK-1.3.268.0](https://vulkan.lunarg.com/sdk/home)
  - 
  **Windows**

```
VulkanSDK-1.2.182.1-Installer.exe --accept-licenses --default-answer --confirm-command install com.lunarg.vulkan.32bit com.lunarg.vulkan.sdl2 com.lunarg.vulkan.glm com.lunarg.vulkan.volk com.lunarg.vulkan.vma com.lunarg.vulkan.debug com.lunarg.vulkan.debug32
```

- **linux (ubuntu jammy)**:

```
    wget -qO- https://packages.lunarg.com/lunarg-signing-key-pub.asc | sudo tee /etc/apt/trusted.gpg.d/lunarg.asc
    sudo wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.3.268-jammy.list https://packages.lunarg.com/vulkan/1.3.268/lunarg-vulkan-1.3.268-jammy.list
    sudo apt update
    sudo apt install vulkan-sdk
```

- compile shaders

```
glslc plus.comp -o plus.spv 
glslc minus.comp -o minus.spv
glslc multiply.comp -o multiply.spv

```

make sure you are using 64bit architecture on Windows

## Usage

```
ppr(.exe) --data_path_hr <path_HR> --data_path_acc <path_ACC> 
optional [
--not_use_gpu 
--load_data_sequentially 
--gpu_workgroup_size <INTEGER>
--minimum_equation_coefficients <FLOAT>
--maximum_equation_coefficient <FLOAT>
--epochs <INTEGER>
--maximum_equation_init_length <INTEGER>
]
```