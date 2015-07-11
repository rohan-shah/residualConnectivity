project(residualConnectivity)
cmake_minimum_required(VERSION 3.1)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake" "${CMAKE_SOURCE_DIR}/..")
set(Boost_USE_STATIC_LIBS	OFF)
set(Boost_USE_MULTITHREADED	ON)
set(Boost_USE_STATIC_RUNTIME	OFF)

option(BUILD_R_PACKAGE "Build R package" ON)
option(BUILD_MATLAB_PACKAGE "Build Matlab package" ON)
option(BUILD_VISUALISATION "Build visualisation code" ON)

if(CMAKE_COMPILER_IS_GNUCC)
	add_definitions("-Wall")
endif()

#Find boost packages
find_package(Boost 1.53.0 COMPONENTS graph program_options system serialization filesystem regex REQUIRED)
find_package(OpenMP REQUIRED)

if(NOT DEFINED EIGEN_INCLUDE_DIR)
	message(FATAL_ERROR "Please point the environment variable EIGEN_INCLUDE_DIR to the include directory of your Eigen3 installation.")
endif()
include_directories("${EIGEN_INCLUDE_DIR}")

if(BUILD_MATLAB_PACKAGE)
	find_package(Matlab COMPONENTS MEX_COMPILER MX_LIBRARY REQUIRED)
endif()
if(BUILD_VISUALISATION)
	#Try config file first
	find_package(igraph CONFIG QUIET)
	if(NOT HAVE_IGRAPH)
		find_package(igraph REQUIRED)
	endif()
	find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)
endif()

if(("${CMAKE_BUILD_TYPE}" STREQUAL "Release") OR ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo"))
	find_package(OpenMP)
endif()

find_package(Mathematica COMPONENTS MathLink)

if(WIN32)
	find_package(mpfr)
	find_package(mpir)

	#If we're on windows, Start copying dlls
	set(BUILD_TYPES Release Debug RelWithDebInfo MinSizeRel)
	set(BOOST_BUILD_TYPES Release Debug Release Release)
	foreach(val RANGE 3)
		list(GET BUILD_TYPES ${val} BUILD_TYPE)
		list(GET BOOST_BUILD_TYPES ${val} BOOST_BUILD_TYPE)
		string(TOUPPER ${BOOST_BUILD_TYPE} BOOST_BUILD_TYPE_UPPER)
		
		#copy boost serialization
		get_filename_component(NAME_PART ${Boost_SERIALIZATION_LIBRARY_${BOOST_BUILD_TYPE_UPPER}} NAME_WE)
		get_filename_component(PATH_PART ${Boost_SERIALIZATION_LIBRARY_${BOOST_BUILD_TYPE_UPPER}} PATH)
		file(COPY ${PATH_PART}/${NAME_PART}.dll DESTINATION ${CMAKE_BINARY_DIR}/${BUILD_TYPE}/)

		#copy boost program_options
		get_filename_component(NAME_PART ${Boost_PROGRAM_OPTIONS_LIBRARY_${BOOST_BUILD_TYPE_UPPER}} NAME_WE)
		get_filename_component(PATH_PART ${Boost_PROGRAM_OPTIONS_LIBRARY_${BOOST_BUILD_TYPE_UPPER}} PATH)
		file(COPY ${PATH_PART}/${NAME_PART}.dll DESTINATION ${CMAKE_BINARY_DIR}/${BUILD_TYPE}/)

		#copy boost graph
		get_filename_component(NAME_PART ${Boost_GRAPH_LIBRARY_${BOOST_BUILD_TYPE_UPPER}} NAME_WE)
		get_filename_component(PATH_PART ${Boost_GRAPH_LIBRARY_${BOOST_BUILD_TYPE_UPPER}} PATH)
		file(COPY ${PATH_PART}/${NAME_PART}.dll DESTINATION ${CMAKE_BINARY_DIR}/${BUILD_TYPE}/)

		#copy boost regex
		get_filename_component(NAME_PART ${Boost_REGEX_LIBRARY_${BOOST_BUILD_TYPE_UPPER}} NAME_WE)
		get_filename_component(PATH_PART ${Boost_REGEX_LIBRARY_${BOOST_BUILD_TYPE_UPPER}} PATH)
		file(COPY ${PATH_PART}/${NAME_PART}.dll DESTINATION ${CMAKE_BINARY_DIR}/${BUILD_TYPE}/)

		#Now for MPFR
		get_target_property(MPFR_LOCATION mpfr "LOCATION_${BOOST_BUILD_TYPE}")
		file(COPY "${MPFR_LOCATION}" DESTINATION "${CMAKE_BINARY_DIR}/${BUILD_TYPE}/")

		#Now for MPIR
		get_target_property(MPIR_LOCATION mpir "LOCATION_${BOOST_BUILD_TYPE}")
		file(COPY "${MPIR_LOCATION}" DESTINATION "${CMAKE_BINARY_DIR}/${BUILD_TYPE}/")
	endforeach()
else()
	#mpfr needs to be listed as an imported target so that we can use target_link_librarie later on. If we don't have this then it just adds a -lmpfr which probably picks up the wrong thing. 
	find_library(mpfr_LIBRARY mpfr)
	add_library(mpfr SHARED IMPORTED)
	set_target_properties(mpfr PROPERTIES IMPORTED_LOCATION ${mpfr_LIBRARY})
endif()
add_subdirectory(residualConnectivityCommon)
add_subdirectory(crudeMC)
add_subdirectory(exhaustiveProbability)
add_subdirectory(exhaustiveSearch)
add_subdirectory(usingBiconnectedSplitting)
add_subdirectory(optimalStateIndependentImportance)
add_subdirectory(transferMatrix)
add_subdirectory(transferMatrixCommon)
add_subdirectory(transferMatrixEstimation)
add_subdirectory(conditionalMC)
add_subdirectory(cancela2002RVR)
add_subdirectory(gridCountSpecificSize)
add_subdirectory(gridCountSpecificSize2)
if(${Mathematica_MathLink_FOUND})
	add_subdirectory(getMinimumReliability)
endif()
add_subdirectory(splittingBasic)
if(BUILD_VISUALISATION)
	add_subdirectory(subObservationVisualiser)
endif()
if(BUILD_MATLAB_PACKAGE)
	add_subdirectory(matlabPackage)
endif()
add_subdirectory(articulationConditioningSplitting)
add_subdirectory(articulationConditioningResampling)
add_subdirectory(expectedVertexCount)

if(BUILD_R_PACKAGE)
	add_subdirectory(RPackage)
	find_package(R REQUIRED)
	if(WIN32)
		install(CODE "execute_process(COMMAND \"${R_COMMAND}\" CMD INSTALL --no-multiarch RPackage/\$\{CMAKE_INSTALL_CONFIG_NAME\} WORKING_DIRECTORY ${PROJECT_BINARY_DIR})")
	else()
		install(CODE "execute_process(COMMAND \"${R_COMMAND}\" CMD INSTALL RPackage/ WORKING_DIRECTORY ${PROJECT_BINARY_DIR})")
	endif()
endif()
