cmake_minimum_required(VERSION 3.11)

set(TARGETNAME gol2)
file(GLOB SOURCES *.cpp)

add_executable(${TARGETNAME} ${SOURCES})
target_link_libraries(${TARGETNAME} libgol)

include_directories(. ${PROJECT_SOURCE_DIR}/src)
