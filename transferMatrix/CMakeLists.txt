cmake_minimum_required(VERSION 3.1)
add_executable(transferMatrix main.cpp)
target_link_libraries(transferMatrix PUBLIC transferMatrixCommon residualConnectivityCommon)

target_compile_options(transferMatrix PRIVATE ${OpenMP_CXX_FLAGS})

target_compile_definitions(transferMatrix PUBLIC BOOST_ALL_NO_LIB BOOST_ALL_DYN_LINK)
target_compile_definitions(transferMatrix PRIVATE USE_OPENMP)
if(WIN32)
	set_target_properties(transferMatrix PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} PDB_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
else()
	target_link_libraries(transferMatrix PRIVATE ${OpenMP_CXX_FLAGS})
endif()
