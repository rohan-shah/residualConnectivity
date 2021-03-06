cmake_minimum_required(VERSION 3.1)
set(CppFiles arguments.cpp argumentsMPFR.cpp context.cpp depth_first_search_restricted.hpp bridges.hpp connected_components_restricted.hpp observation.cpp isSingleComponentWithRadius.cpp obs/withSub.cpp subObs/subObs.cpp subObsTypes.cpp subObs/basic.cpp obs/basic.cpp binaryDataSet.cpp observationCollection.cpp serializeGMP.hpp empiricalDistribution.cpp observationTree.cpp subObs/withWeight.cpp subObs/withFinalConditioning.hpp constructSubGraph.cpp subObs/articulationConditioning.cpp obs/articulationConditioning.cpp conditionArticulation.cpp outputObject.cpp aliasMethod.cpp calculateFactorials.cpp constructGraphs.cpp obs/articulationConditioningSameCount.cpp subObs/articulationConditioningSameCount.cpp subObs/articulationConditioningSameCountImportance.cpp obs/articulationConditioningSameCountImportance.cpp subObs/articulationConditioningSameCountImportance.cpp obs/articulationConditioningImportance.cpp subObs/articulationConditioningImportance.cpp)
set(HeaderFiles arguments.h argumentsMPFR.h context.h observation.h isSingleComponentWithRadius.h lexographicOrder.h includeMPFRResidualConnectivity.h obs/withSub.h obs/getSubObservation.hpp subObsTypes.h subObs/getObservation.hpp subObs/subObs.h subObs/basic.h obs/basic.h vertexState.h binaryDataSet.h observationCollection.h observationTree.h subObs/withWeight.h constructSubGraph.h conditionArticulation.h obs/articulationConditioning.h outputObject.h aliasMethod.h calculateFactorials.h constructGraphs.h obs/articulationConditioningSameCount.h findFixedOnVisitor.h subObs/articulationConditioningSameCount.h filterByStateMask.h subObs/articulationConditioningSameCountImportance.h obs/articulationConditioningSameCountImportance.h subObs/articulationConditioningSameCountImportance.h obs/articulationConditioningImportance.h subObs/articulationConditioningImportance.h)
if(WIN32)
	list(APPEND HeaderFiles windowsConsoleOutput.h)
	list(APPEND CppFiles windowsConsoleOutput.cpp)
endif()
if(BUILD_VISUALISATION)
	list(APPEND CppFiles zoomGraphicsView.cpp)
	list(APPEND HeaderFiles zoomGraphicsView.h)
endif()

add_library(residualConnectivityCommon STATIC ${CppFiles} ${HeaderFiles})
target_include_directories(residualConnectivityCommon PUBLIC ${Boost_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/residualConnectivityCommon ${IGRAPH_INCLUDES})
target_link_libraries(residualConnectivityCommon PUBLIC ${IGRAPH_LIBRARIES} ${Boost_LIBRARIES} sampling)

if(USE_OPENMP)
	find_package(OpenMP REQUIRED)
	target_compile_definitions(residualConnectivityCommon PRIVATE USE_OPENMP)
	target_compile_options(residualConnectivityCommon PRIVATE ${OpenMP_CXX_FLAGS})
endif()

if(BUILD_VISUALISATION)
	target_link_libraries(residualConnectivityCommon PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)
endif()
target_compile_definitions(residualConnectivityCommon PUBLIC BOOST_ALL_NO_LIB BOOST_ALL_DYN_LINK)
target_link_libraries(residualConnectivityCommon PUBLIC mpfr)
if(WIN32)
	set_target_properties(residualConnectivityCommon PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	target_link_libraries(residualConnectivityCommon PUBLIC mpir)
else()
	target_compile_features(residualConnectivityCommon PUBLIC cxx_rvalue_references)
endif()
