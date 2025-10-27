include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Create test executable
add_executable(graph_tests
  graph_test.cpp
  minheap_test.cpp
)

target_link_libraries(graph_tests
  GTest::gtest_main
  graph_lib
)

include(GoogleTest)
gtest_discover_tests(graph_tests)
