add_executable(test_bin "${CMAKE_SOURCE_DIR}/test/test_bin.cpp")
add_executable(test_slot_heap "${CMAKE_SOURCE_DIR}/test/test_slot_heap.cpp")

include(GoogleTest)

foreach(TEST_NAME test_bin test_slot_heap)
    target_link_libraries(${TEST_NAME} gtest_main gmock)
    target_include_directories(${TEST_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/solution")
    include(GoogleTest)
    gtest_discover_tests(${TEST_NAME})
endforeach()