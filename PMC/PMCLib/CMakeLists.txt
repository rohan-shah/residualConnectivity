cmake_minimum_required(VERSION 3.1)
add_library(PMCLib STATIC PMCLib.cpp PMCLib.h)
target_link_libraries(PMCLib PUBLIC residualConnectivityCommon)
target_include_directories(PMCLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
	set_target_properties(PMCLib PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
