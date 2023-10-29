cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)
if (APPLE)
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

find_package(OpenCL REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp src/RecordHR.h src/RecordHR.cpp
        src/RecordACC.h
        src/Loader.h
        src/Loader.cpp
        src/RecordACC.cpp)


if (APPLE)
    target_link_libraries(ppr OpenCL)
else (
)
    target_link_libraries(ppr PRIVATE OpenCL)
endif (APPLE)
