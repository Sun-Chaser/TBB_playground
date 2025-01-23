cmake_minimum_required(VERSION 3.10)

# Project name and C++ standard
project(TBBExample)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find the TBB library
find_package(TBB REQUIRED)

# Add the executable
add_executable(apt_testing Stream_Pipeline.cpp)

# Link the TBB library to the executable
target_link_libraries(apt_testing PRIVATE TBB::tbb)
