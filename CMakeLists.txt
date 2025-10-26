cmake_minimum_required(VERSION 4.0)
project(PathVista)

set(CMAKE_CXX_STANDARD 14)

include_directories(PathVista)

add_executable(PathVista
        PathVista/MinHeap.tpp
        PathVista/MinHeap.h)
