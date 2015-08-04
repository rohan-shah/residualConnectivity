cmake_minimum_required(VERSION 3.1)
add_executable(gridCountSpecificSize2 main.cpp)
target_link_libraries(gridCountSpecificSize2 PUBLIC residualConnectivityCommon)

target_compile_options(gridCountSpecificSize2 PRIVATE ${OpenMP_CXX_FLAGS})

target_compile_definitions(gridCountSpecificSize2 PUBLIC BOOST_ALL_NO_LIB BOOST_ALL_DYN_LINK)
target_compile_definitions(gridCountSpecificSize2 PRIVATE USE_OPENMP)
if(WIN32)
	set_target_properties(gridCountSpecificSize2 PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
else()
	target_link_libraries(gridCountSpecificSize2 PRIVATE ${OpenMP_CXX_FLAGS})
endif()
