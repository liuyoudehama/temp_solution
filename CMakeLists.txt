cmake_minimum_required(VERSION 3.15)

project(assessment1)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(LoadingData  ${SOURCES})
