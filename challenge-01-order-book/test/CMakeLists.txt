add_executable(bst_test "${CMAKE_SOURCE_DIR}/solution/bst_impl.cpp" "${CMAKE_SOURCE_DIR}/test/ob_test.cpp")

target_link_libraries(bst_test gtest_main)
target_include_directories(bst_test PRIVATE "${CMAKE_SOURCE_DIR}/solution")

include(GoogleTest)
gtest_discover_tests(bst_test)