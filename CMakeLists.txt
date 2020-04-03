cmake_minimum_required(VERSION 3.11)

set(CMAKE_CXX_STANDARD 17)
project(gol2 CXX)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CompilerFlags         
	CMAKE_CXX_FLAGS
	CMAKE_CXX_FLAGS_DEBUG
	CMAKE_CXX_FLAGS_RELEASE
	CMAKE_C_FLAGS
	CMAKE_C_FLAGS_DEBUG
	CMAKE_C_FLAGS_RELEASE
	) 
foreach(CompilerFlag ${CompilerFlags})   
	string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}") 
endforeach()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchGTest.cmake)
include(GoogleTest)

enable_testing()

add_subdirectory(src)
