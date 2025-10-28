cmake_minimum_required(VERSION 3.15)
project(assessment1)

# std >= c++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Boost 路径（适配 Homebrew，手动指定可选）
set(BOOST_ROOT "/usr/local/Cellar/boost/1.82.0_1")
set(BOOST_INCLUDEDIR "/usr/local/Cellar/boost/1.82.0_1/include")
set(BOOST_LIBRARYDIR "/usr/local/Cellar/boost/1.82.0_1/lib")

# 查找 Boost program_options
find_package(Boost REQUIRED COMPONENTS program_options)

message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost_PROGRAM_OPTIONS_LIBRARY: ${Boost_PROGRAM_OPTIONS_LIBRARY}")

if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost not found!")
endif()

# prerequisite
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

message(STATUS "PQXX_FOUND: ${PQXX_FOUND}")
message(STATUS "PQXX_INCLUDE_DIRS: ${PQXX_INCLUDE_DIRS}")
message(STATUS "PQXX_LIBRARIES: ${PQXX_LIBRARIES}")
message(STATUS "PQXX_LIBRARY_DIRS: ${PQXX_LIBRARY_DIRS}")

# lib
add_library(DBtransaction STATIC
    src/InspectionGroupRepository.cpp
    src/InspectionRegionRepository.cpp
)
target_include_directories(DBtransaction PUBLIC ${CMAKE_SOURCE_DIR}/include ${PQXX_INCLUDE_DIRS})

# file(GLOB_RECURSE SOURCES src/*.cpp)


# main
add_executable(LoadingData  src/main.cpp)

target_include_directories(LoadingData PRIVATE ${CMAKE_SOURCE_DIR}/include ${PQXX_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
target_link_directories(LoadingData PRIVATE ${PQXX_LIBRARY_DIRS} ${BOOST_LIBRARYDIR})
target_link_libraries(LoadingData PRIVATE ${PQXX_LIBRARIES} ${Boost_PROGRAM_OPTIONS_LIBRARY} DBtransaction)