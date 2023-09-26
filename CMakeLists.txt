cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
set(PKG_CONFIG_PATH "/usr/local/opt/opencl-clhpp-headers/share/pkgconfig")
set(CPPFLAGS "-I/usr/local/opt/opencl-clhpp-headers/include")
set(CMAKE_CXX_FLAGS  "-Wall -Wextra -pedantic -framework OpenCL")
project(ppr-semestralka)


find_package(OpenGL REQUIRED)

add_executable(ppr-semestralka src/main.cpp src/RecordHR.h src/RecordHR.cpp
        src/RecordACC.h
        src/Loader.h
        src/Loader.cpp
        src/RecordACC.cpp)

#target_include_directories(ppr-semestralka  ${OPENCL_INCLUDE_DIRS} )
target_link_libraries(ppr-semestralka ${OPENCL_LIBRARIES})