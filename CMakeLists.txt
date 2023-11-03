cmake_minimum_required(VERSION 3.28)


project(ppr)

set(CMAKE_CXX_STANDARD 20)


set(CMAKE_CXX_SCAN_FOR_MODULES YES)
# find Vulkan SDK
find_package( Vulkan REQUIRED )

include_directories(${VULKAN_SDK}/Inlude)


set(target_sources
        main.cpp RecordHR.h RecordHR.cpp
        RecordACC.h
        Loader.h
        Loader.cpp
        RecordACC.cpp VulkanGpu.cpp VulkanGpu.h
)
list(TRANSFORM target_sources PREPEND "src/")

# link Vulkan C++ module into your project
add_executable(${PROJECT_NAME} ${target_sources})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)


#
#
#project(ppr)
#
#if (APPLE)
#    set(PKG_CONFIG_PATH "/usr/local/opt/opencl-clhpp-headers/share/pkgconfig")
#    set(CPPFLAGS "-I/usr/local/opt/opencl-clhpp-headers/include")
#    set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -framework OpenCL")
#else ()
#    set(OpenCL_LIBRARY OpenCL-SDK/lib)
#    set(OpenCL_INCLUDE_DIR OpenCL-SDK/include)
#    link_directories(OpenCL-SDK/lib)
#    include_directories(OpenCL-SDK/include)
#
#    set(OpenCL_LIBRARY OpenCL-SDK/lib)
#    set(OpenCL_INCLUDE_DIR OpenCL-SDK/include)
#    link_directories(OpenCL-SDK/lib)
#    include_directories(OpenCL-SDK/include)
#
#    set(OpenCL_DIR OpenCL-SDK)
#
#    find_package(OpenCL REQUIRED)
#endif (APPLE)
#
##set(CMAKE_OSX_ARCHITECTURES x86_64)
#
#
#
#
#
#
#add_executable(${PROJECT_NAME} src/main.cpp src/RecordHR.h src/RecordHR.cpp
#        src/RecordACC.h
#        src/Loader.h
#        src/Loader.cpp
#        src/RecordACC.cpp)
#
#
#if (APPLE)
#
#else ()
#    target_link_libraries(ppr PRIVATE OpenCL)
#endif (APPLE)
#
#
