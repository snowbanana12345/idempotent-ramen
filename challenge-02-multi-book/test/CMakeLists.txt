add_executable(default_test "${CMAKE_SOURCE_DIR}/solution/default_impl.cpp" "${CMAKE_SOURCE_DIR}/test/test_bin.cpp")
add_executable(tree_test "${CMAKE_SOURCE_DIR}/solution/tree_impl.cpp" "${CMAKE_SOURCE_DIR}/test/test_bin.cpp")

include(GoogleTest)

foreach(TEST_NAME default_test tree_test)
    target_link_libraries(${TEST_NAME} gtest_main gmock)
    target_include_directories(${TEST_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/solution")
    include(GoogleTest)
    gtest_discover_tests(${TEST_NAME})
endforeach()