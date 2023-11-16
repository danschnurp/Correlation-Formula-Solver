cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)
if (APPLE)
    set(CMAKE_OSX_ARCHITECTURES x86_64)
    set(PKG_CONFIG_PATH "/usr/local/opt/opencl-clhpp-headers/share/pkgconfig")
    set(CPPFLAGS "-I/usr/local/opt/opencl-clhpp-headers/include")
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -framework OpenCL")
else ()
    set(OpenCL_LIBRARY OpenCL-SDK/lib)
    set(OpenCL_INCLUDE_DIR OpenCL-SDK/include)
    link_directories(OpenCL-SDK/lib)
    include_directories(OpenCL-SDK/include)
endif (APPLE)

project(ppr)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenCL REQUIRED)

set(CMAKE_CXX_SCAN_FOR_MODULES YES)
## find Vulkan SDK
#find_package( Vulkan REQUIRED )


set(target_sources
        main.cpp
        io/RecordHR.h io/RecordHR.cpp
        io/RecordACC.h io/RecordACC.cpp
        io/Loader.h io/Loader.cpp
        gpu/OpenclGPU.h gpu/OpenclGPU.cpp
        preprocessing/utils.cpp preprocessing/utils.h
        genetic_alg/Node.cpp genetic_alg/Node.h
        genetic_alg/Equation.cpp genetic_alg/Equation.h
        genetic_alg/Population.cpp genetic_alg/Population.h)

list(TRANSFORM target_sources PREPEND "src/")


add_executable(${PROJECT_NAME} ${target_sources})

#target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

if (APPLE)
    # Include OpenCL headers
    target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCL_INCLUDE_DIRS})

    # Link against OpenCL library
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCL_LIBRARIES})
else (
)
    target_link_libraries(ppr PRIVATE OpenCL)
endif (APPLE)