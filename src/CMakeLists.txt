# Create library for testing
add_library(graph_lib
    graph.cpp
    graph.h
    MinHeap.h
    MinHeap.tpp
)

target_include_directories(graph_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Create main executable
add_executable(PathVista main.cpp)
target_link_libraries(PathVista graph_lib)
