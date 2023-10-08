cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_PREFIX_PATH "./OpenCL-Headers/build;./OpenCL-ICD-Loader/build;./OpenCL-CLHPP/build" /path/to/opencl/app)

#set(CMAKE_CXX_FLAGS "-Wall  -Wpedantic")
project(ppr)

find_package(OpenCLHeaders REQUIRED)
find_package(OpenCLICDLoader REQUIRED)
find_package(OpenCLHeadersCpp REQUIRED)


add_executable(${PROJECT_NAME} src/main.cpp src/RecordHR.h src/RecordHR.cpp
        src/RecordACC.h
        src/Loader.h
        src/Loader.cpp
        src/RecordACC.cpp)

target_link_libraries(ppr PRIVATE OpenCL::Headers OpenCL::OpenCL OpenCL::HeadersCpp)
