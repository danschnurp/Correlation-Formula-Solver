cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
project(pprka)


add_executable(pprka_svg src/svg_gen.cpp)

project(pprka_svg)

add_executable(pprka src/main.cpp src/RecordHR.h src/RecordHR.cpp)
