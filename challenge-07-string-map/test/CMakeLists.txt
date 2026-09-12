add_executable(main_test "${CMAKE_SOURCE_DIR}/test/test_bin.cpp")

include(GoogleTest)

foreach(TEST_NAME main_test)
    target_link_libraries(${TEST_NAME} gtest_main)
    target_include_directories(${TEST_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/solution")
    include(GoogleTest)
    gtest_discover_tests(${TEST_NAME})
endforeach()