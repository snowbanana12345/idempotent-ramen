# add_executable(test_bin "${CMAKE_SOURCE_DIR}/solution/solution.cpp" "${CMAKE_SOURCE_DIR}/test/test_bin.cpp")
# add_executable(test_bin "${CMAKE_SOURCE_DIR}/solution/heap_impl.cpp" "${CMAKE_SOURCE_DIR}/test/test_bin.cpp")
# add_executable(test_bin "${CMAKE_SOURCE_DIR}/solution/micro_bucket_impl.cpp" "${CMAKE_SOURCE_DIR}/test/test_bin.cpp")
add_executable(test_bin "${CMAKE_SOURCE_DIR}/solution/slot_heap_impl.cpp" "${CMAKE_SOURCE_DIR}/test/test_bin.cpp")

add_executable(slot_hmp "${CMAKE_SOURCE_DIR}/test/test_slot_heap_map.cpp")

include(GoogleTest)

foreach(TEST_NAME test_bin slot_hmp)
    target_link_libraries(${TEST_NAME} gtest_main gmock)
    target_include_directories(${TEST_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/solution")
    include(GoogleTest)
    gtest_discover_tests(${TEST_NAME})
endforeach()