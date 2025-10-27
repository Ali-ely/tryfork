cmake_minimum_required(VERSION 3.14)
project(PathVista)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source directory
add_subdirectory(src)

# Enable testing
enable_testing()

# Add test directory
add_subdirectory(test)
