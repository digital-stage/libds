cmake_minimum_required(VERSION 3.16.0)

project(DigitalStageTests
        LANGUAGES CXX
        VERSION 0.1
        DESCRIPTION "Tests for digital stage library")


# Locate GTest
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Link runTests with what we want to test and the GTest and pthread library
add_executable(tests test_main.cc test1.cc)
target_link_libraries(tests ${GTEST_LIBRARIES} pthread)
gtest_discover_tests(tests)