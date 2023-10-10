cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
if (APPLE)
set(PKG_CONFIG_PATH "/usr/local/opt/opencl-clhpp-headers/share/pkgconfig")
set(CPPFLAGS "-I/usr/local/opt/opencl-clhpp-headers/include")
set(CMAKE_CXX_FLAGS  "-Wall -Wextra -pedantic -framework OpenCL")
else ()
set(CMAKE_PREFIX_PATH "./OpenCL-Headers/build;./OpenCL-ICD-Loader/build;./OpenCL-CLHPP/build" ./)
endif (APPLE)
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


if (APPLE)
target_link_libraries(ppr ${OPENCL_LIBRARIES})
else (
)
    target_link_libraries(ppr PRIVATE OpenCL::Headers OpenCL::OpenCL OpenCL::HeadersCpp)
endif (APPLE)