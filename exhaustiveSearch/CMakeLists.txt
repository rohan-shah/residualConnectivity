cmake_minimum_required(VERSION 3.1)
add_executable(exhaustiveSearch exhaustiveSearch.cpp)
target_link_libraries(exhaustiveSearch PUBLIC residualConnectivityCommon)
if(NOT WIN32)
	target_link_libraries(exhaustiveSearch PRIVATE ${OpenMP_CXX_FLAGS})
endif()

target_compile_options(exhaustiveSearch PRIVATE ${OpenMP_CXX_FLAGS})

target_compile_definitions(exhaustiveSearch PUBLIC BOOST_ALL_NO_LIB BOOST_ALL_DYN_LINK)
target_compile_definitions(exhaustiveSearch PRIVATE USE_OPENMP)
if(WIN32)
	set_target_properties(exhaustiveSearch PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/x64/ PDB_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/x64/)
endif()
