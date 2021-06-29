cmake_minimum_required (VERSION 3.8)

project ("BuildSys")

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable (buildsys "buildsys.cpp")
