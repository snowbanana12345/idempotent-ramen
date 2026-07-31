# add_executable(default_test "${CMAKE_SOURCE_DIR}/solution/default_impl.cpp" "${CMAKE_SOURCE_DIR}/test/test_bin.cpp")
add_executable(tree_test "${CMAKE_SOURCE_DIR}/solution/tree_impl.cpp" "${CMAKE_SOURCE_DIR}/test/test_bin.cpp")

add_executable(queue_order_test "${CMAKE_SOURCE_DIR}/test/test_queue_order.cpp")
add_executable(agg_book_test "${CMAKE_SOURCE_DIR}/test/test_agg_book.cpp")

include(GoogleTest)

foreach(TEST_NAME tree_test agg_book_test queue_order_test)
    target_link_libraries(${TEST_NAME} gtest_main gmock)
    target_include_directories(${TEST_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/solution")
    include(GoogleTest)
    gtest_discover_tests(${TEST_NAME})
endforeach()