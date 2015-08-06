cmake_minimum_required(VERSION 3.1)
add_library(stochasticEnumerationLib STATIC stochasticEnumerationLib.cpp stochasticEnumerationLib.h)
target_link_libraries(stochasticEnumerationLib PUBLIC residualConnectivityCommon)
target_include_directories(stochasticEnumerationLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32)
	set_target_properties(stochasticEnumerationLib PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
