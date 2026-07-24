add_executable(bst_test "${CMAKE_SOURCE_DIR}/solution/bst_impl.cpp" "${CMAKE_SOURCE_DIR}/test/ob_test.cpp")
add_executable(heap_test "${CMAKE_SOURCE_DIR}/solution/heap_impl.cpp" "${CMAKE_SOURCE_DIR}/test/ob_test.cpp")

include(GoogleTest)

foreach(TEST_NAME bst_test heap_test)
    target_link_libraries(${TEST_NAME} gtest_main)
    target_include_directories(${TEST_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/solution")
    include(GoogleTest)
    gtest_discover_tests(${TEST_NAME})
endforeach()