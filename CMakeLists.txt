cmake_minimum_required(VERSION 2.4)

project(package/libhuffman C CXX)
message(STATUS "Entering into ${PROJECT_SOURCE_DIR} ..")

# We need C++ 11
if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 3.1)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED on)
else(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 3.1)
    set(CMAKE_CXX_FLAGS "-std=c++11")
endif(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} GREATER 3.1)

# Recurse into subdirectories
add_subdirectory(libhuffman)    # Library
add_subdirectory(demo)          # Application
