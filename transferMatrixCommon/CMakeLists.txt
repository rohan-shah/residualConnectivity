cmake_minimum_required(VERSION 3.1)
set(HeaderFiles constructMatrices.h states.h countSubgraphsBySize.h countSubgraphsCommon.h countSubgraphs.h isConnectedState.h customizedEigen.h)
set(CppFiles constructMatrices.cpp states.cpp countSubgraphsBySize.cpp countSubgraphsCommon.cpp countSubgraphs.cpp isConnectedState.cpp subgraphMacros.hpp customizedEigen.cpp)
add_library(transferMatrixCommon STATIC ${HeaderFiles} ${CppFiles})
target_compile_definitions(transferMatrixCommon PUBLIC BOOST_ALL_NO_LIB BOOST_ALL_DYN_LINK)
target_compile_definitions(transferMatrixCommon PRIVATE USE_OPENMP)

target_compile_options(transferMatrixCommon PRIVATE ${OpenMP_CXX_FLAGS})

target_include_directories(transferMatrixCommon PUBLIC ${Boost_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/transferMatrixCommon)

target_link_libraries(transferMatrixCommon PUBLIC ${Boost_LIBRARIES} residualConnectivityCommon)
if(WIN32)
	set_target_properties(transferMatrixCommon PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR} PDB_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
else()
	target_compile_features(transferMatrixCommon PUBLIC cxx_rvalue_references)
	target_link_libraries(transferMatrixCommon PRIVATE ${OpenMP_CXX_FLAGS})
endif()
