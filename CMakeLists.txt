cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_OSX_ARCHITECTURES x86_64)

set(OpenCL_LIBRARY OpenCL-SDK/lib)
set(OpenCL_INCLUDE_DIR OpenCL-SDK/include)
link_directories(OpenCL-SDK/lib)
include_directories(OpenCL-SDK/include)

set(OpenCL_DIR OpenCL-SDK)

project(ppr)

find_package(OpenCL REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp src/RecordHR.h src/RecordHR.cpp
        src/RecordACC.h
        src/Loader.h
        src/Loader.cpp
        src/RecordACC.cpp)



target_link_libraries(ppr PRIVATE OpenCL)

