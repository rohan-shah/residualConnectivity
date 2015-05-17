cmake_minimum_required(VERSION 3.1)
add_executable(gridCountSpecificSize main.cpp)
target_link_libraries(gridCountSpecificSize PUBLIC residualConnectivityCommon)

target_compile_options(gridCountSpecificSize PRIVATE ${OpenMP_CXX_FLAGS})

target_compile_definitions(gridCountSpecificSize PUBLIC BOOST_ALL_NO_LIB BOOST_ALL_DYN_LINK)
target_compile_definitions(gridCountSpecificSize PRIVATE USE_OPENMP)
if(WIN32)
	set_target_properties(gridCountSpecificSize PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} PDB_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
else()
	target_link_libraries(gridCountSpecificSize PRIVATE ${OpenMP_CXX_FLAGS})
endif()
