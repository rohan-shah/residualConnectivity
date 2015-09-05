cmake_minimum_required(VERSION 3.1)
add_executable(PMC PMC.cpp)
target_link_libraries(PMC PUBLIC residualConnectivityCommon monteCarloMethods)

if(WIN32)
	set_target_properties(PMC PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()
