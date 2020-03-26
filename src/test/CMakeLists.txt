cmake_minimum_required(VERSION 3.11)

set(TARGETNAME goltest)
file(GLOB SOURCES *.cpp)

add_executable(${TARGETNAME} ${SOURCES})
target_link_libraries(${TARGETNAME} libgol gtest gmock gtest_main)

include_directories(. ${PROJECT_SOURCE_DIR}/src ${GTEST_INCLUDE_DIR})

message("DDDDDD ${GTEST_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src")

gtest_add_tests(
    TARGET ${TARGETNAME}
    TEST_LIST ${TARGETNAME}List
)

