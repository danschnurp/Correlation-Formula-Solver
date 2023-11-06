cmake_minimum_required(VERSION 3.26)


project(ppr)

set(CMAKE_CXX_STANDARD 20)


set(CMAKE_CXX_SCAN_FOR_MODULES YES)
# find Vulkan SDK
find_package( Vulkan REQUIRED )

include_directories(${VULKAN_SDK}/Inlude)

set(target_sources
        main.cpp
        io/RecordHR.h io/RecordHR.cpp
        io/RecordACC.h io/RecordACC.cpp
        io/Loader.h io/Loader.cpp
        VulkanGpu.cpp VulkanGpu.h
        preprocessing/utils.cpp preprocessing/utils.h)

list(TRANSFORM target_sources PREPEND "src/")

# link Vulkan C++ module into your project
add_executable(${PROJECT_NAME} ${target_sources})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)
