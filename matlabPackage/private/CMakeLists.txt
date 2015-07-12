project(residualConnectivity)
set(MFILES getGraph.m)

set(MEXFILES crudeMCImpl.cpp conditionalMCImpl.cpp exhaustiveSearchImpl.cpp)
set(MEXFILE_DEPENDENCIES crudeMCLib conditionalMCLib exhaustiveSearchLib)
list(LENGTH MEXFILES N_MEXFILES)
MATH(EXPR N_MEXFILES ${N_MEXFILES}-1)

add_custom_target(copyMatlabPackagePrivate ALL)

if(WIN32)
	set(BUILD_TYPES Release Debug RelWithDebInfo MinSizeRel)
	set(BOOST_BUILD_TYPES Release Debug Release Release)
	#Set up MEX targets
	foreach(counter RANGE ${N_MEXFILES})
		list(GET MEXFILES ${counter} MEXFILE)
		list(GET MEXFILE_DEPENDENCIES ${counter} MEXFILE_DEPENDENCY)
		get_filename_component(MEXFILE_NAME "${MEXFILE}" NAME_WE)
		matlab_add_mex(NAME ${MEXFILE_NAME}Matlab SRC ${MEXFILE} convertGraph.cpp LINK_TO ${MEXFILE_DEPENDENCY} OUTPUT_NAME ${MEXFILE_NAME})
	endforeach()
	#Iterate over build types
	foreach(counter RANGE ${N_MEXFILES})
		list(GET BUILD_TYPES ${counter} BUILD_TYPE)
		#We also have a boost build type which tells us which dll to copy
		list(GET BOOST_BUILD_TYPES ${counter} BOOST_BUILD_TYPE)

		#Correct case
		string(TOUPPER "${BUILD_TYPE}" BUILD_TYPE_TO_UPPER)
		string(TOUPPER "${BOOST_BUILD_TYPE}" BOOST_BUILD_TYPE_TO_UPPER)
		#Copy .m files
		foreach(MFILE IN LISTS MFILES)
			add_custom_command(TARGET copyMatlabPackagePrivate COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${MFILE} ${CMAKE_BINARY_DIR}/matlabPackage/${BUILD_TYPE}/private/${MFILE})
		endforeach()
		#Set output destinations for MEX files
		foreach(MEXFILE IN LISTS MEXFILES)
			get_filename_component(MEXFILE_NAME "${MEXFILE}" NAME_WE)
			set_target_properties(${MEXFILE_NAME}Matlab PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${BUILD_TYPE_TO_UPPER} ${PROJECT_BINARY_DIR}/../../matlabPackage/${BUILD_TYPE}/private)
		endforeach()

		#Copy boost serialization DLL for MEX files
		get_filename_component(NAME_PART ${Boost_SERIALIZATION_LIBRARY_${BOOST_BUILD_TYPE_TO_UPPER}} NAME_WE)
		get_filename_component(PATH_PART ${Boost_SERIALIZATION_LIBRARY_${BOOST_BUILD_TYPE_TO_UPPER}} PATH)
		file(COPY ${PATH_PART}/${NAME_PART}.dll DESTINATION "${CMAKE_BINARY_DIR}/matlabPackage/${BUILD_TYPE}/private/")

		#Copy boost graph DLL for MEX files
		get_filename_component(NAME_PART ${Boost_GRAPH_LIBRARY_${BOOST_BUILD_TYPE_TO_UPPER}} NAME_WE)
		get_filename_component(PATH_PART ${Boost_GRAPH_LIBRARY_${BOOST_BUILD_TYPE_TO_UPPER}} PATH)
		file(COPY ${PATH_PART}/${NAME_PART}.dll DESTINATION "${CMAKE_BINARY_DIR}/matlabPackage/${BUILD_TYPE}/private/")

		#Copy boost regex DLL for MEX files
		get_filename_component(NAME_PART ${Boost_REGEX_LIBRARY_${BOOST_BUILD_TYPE_TO_UPPER}} NAME_WE)
		get_filename_component(PATH_PART ${Boost_REGEX_LIBRARY_${BOOST_BUILD_TYPE_TO_UPPER}} PATH)
		file(COPY ${PATH_PART}/${NAME_PART}.dll DESTINATION "${CMAKE_BINARY_DIR}/matlabPackage/${BUILD_TYPE}/private/")

		#Copy MPFR dll
		get_target_property(MPFR_LOCATION mpfr "LOCATION_${BOOST_BUILD_TYPE}")
		file(COPY "${MPFR_LOCATION}" DESTINATION "${CMAKE_BINARY_DIR}/matlabPackage/${BUILD_TYPE}/private/")

		#Copy MPIR dll
		get_target_property(MPIR_LOCATION mpir "LOCATION_${BOOST_BUILD_TYPE}")
		file(COPY "${MPIR_LOCATION}" DESTINATION "${CMAKE_BINARY_DIR}/matlabPackage/${BUILD_TYPE}/private/")
	endforeach()
else()
	foreach(MFILE IN LISTS MFILES)
		add_custom_command(TARGET copyMatlabPackagePrivate COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/${MFILE} ${CMAKE_BINARY_DIR}/matlabPackage/private/)
	endforeach()
	#On linux matlab ships with its own libstdc++, which we don't necessarily want to pull in. So we find our own libstd (the matlab one is set using LD_LIBRARY_PATH, so we won't pick it up here)
	find_library(LIBSTD_FOR_MEX stdc++)
	foreach(counter RANGE (${N_MEXFILES}-1))
		list(GET MEXFILES ${counter} MEXFILE)
		list(GET MEXFILE_DEPENDENCIES ${counter} MEXFILE_DEPENDENCY)
		get_filename_component(MEXFILE_NAME "${MEXFILE}" NAME_WE)
		matlab_add_mex(NAME ${MEXFILE_NAME}Matlab SRC ${MEXFILE} convertGraph.cpp LINK_TO ${MEXFILE_DEPENDENCY} OUTPUT_NAME ${MEXFILE_NAME})
	endforeach()
endif()