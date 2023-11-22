cmake_minimum_required(VERSION 3.22)


project(ppr)

set(CMAKE_CXX_STANDARD 20)


set(CMAKE_CXX_SCAN_FOR_MODULES YES)
# find Vulkan SDK
find_package(Vulkan REQUIRED)

include_directories(${VULKAN_SDK}/Inlude)

set(target_sources
        main.cpp
        io/RecordHR.h io/RecordHR.cpp
        io/RecordACC.h io/RecordACC.cpp
        io/Loader.h io/Loader.cpp
        preprocessing/utils.cpp preprocessing/utils.h
        genetic_alg/Node.cpp genetic_alg/Node.h
        genetic_alg/Equation.cpp genetic_alg/Equation.h
        genetic_alg/Population.cpp genetic_alg/Population.h
        gpu/ComputationUnit.cpp gpu/ComputationUnit.h
        gpu/demo/vulkan_helpers.cpp
        gpu/demo/vulkan_helpers.hpp)

list(TRANSFORM target_sources PREPEND "src/")

# link Vulkan C++ module into your project
add_executable(${PROJECT_NAME} ${target_sources})
target_link_libraries(${PROJECT_NAME} Vulkan::Vulkan)

