cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
#set(PKG_CONFIG_PATH "/usr/local/opt/opencl-clhpp-headers/share/pkgconfig")
#set(CPPFLAGS "-I/usr/local/opt/opencl-clhpp-headers/include")

set(OpenCL_LIBRARY "C:/Users/dartix/.vcpkg-clion/vcpkg/packages/opencl_x64-windows/bin")
set(OpenCL_DIR "C:/Users/dartix/.vcpkg-clion/vcpkg/packages/opencl_x64-windows/share/opencl")
set(OpenCL_INCLUDE_DIR "C:/Users/dartix/.vcpkg-clion/vcpkg/packages/opencl_x64-windows/include")


set(CMAKE_CXX_FLAGS "-Wall  -pedantic")
project(ppr)


find_package(OpenCL REQUIRED)


add_executable(ppr src/main.cpp src/RecordHR.h src/RecordHR.cpp
        src/RecordACC.h
        src/Loader.h
        src/Loader.cpp
        src/RecordACC.cpp)

target_include_directories(ppr PRIVATE ${OpenCL_INCLUDE_DIRS})
#target_link_libraries(main PRIVATE OpenCL::OpenCL)