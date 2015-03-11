cmake_minimum_required(VERSION 3.1)
add_executable(transferMatrixEstimation main.cpp)
target_link_libraries(transferMatrixEstimation PUBLIC transferMatrixCommon residualConnectivityCommon)
target_link_libraries(transferMatrixEstimation PRIVATE ${OpenMP_CXX_FLAGS})

target_compile_options(transferMatrixEstimation PRIVATE ${OpenMP_CXX_FLAGS})

target_compile_definitions(transferMatrixEstimation PUBLIC BOOST_ALL_NO_LIB BOOST_ALL_DYN_LINK)
target_compile_definitions(transferMatrixEstimation PRIVATE USE_OPENMP)
if(WIN32)
	set_target_properties(transferMatrixEstimation PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/x64/ PDB_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/x64/)
endif()
