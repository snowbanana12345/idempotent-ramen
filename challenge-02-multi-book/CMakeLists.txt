cmake_minimum_required(VERSION 3.20)
project(challenge-02-multi-book LANGUAGES CXX)
include(FetchContent)

add_executable(benchmark benchmark.cpp solution/default_impl.cpp)
# add_executable(benchmark benchmark.cpp solution/tree_impl.cpp)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../common/challenge.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/../common/challenge.cmake)
elseif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/common/challenge.cmake)
    include(${CMAKE_CURRENT_SOURCE_DIR}/common/challenge.cmake)
endif()

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_subdirectory(test)