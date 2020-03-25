cmake_minimum_required(VERSION 3.11)

set(TARGETNAME libgol)
file(GLOB SOURCES *.cpp)

add_library(${TARGETNAME} ${SOURCES})
