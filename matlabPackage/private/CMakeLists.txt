set(MEXFILES crudeMCImpl.cpp conditionalMCImpl.cpp countConnectedSubgraphsBySizeImpl.cpp recursiveVarianceReductionImpl.cpp)
set(MEXFILE_DEPENDENCIES monteCarloMethods monteCarloMethods exactMethods monteCarloMethods)
list(LENGTH MEXFILES N_MEXFILES)
MATH(EXPR N_MEXFILES ${N_MEXFILES}-1)

#Target for files that should always be copied
set(copyMatlabPackagePrivateDependencies)
if(WIN32)
	#Set up MEX targets
	foreach(counter RANGE ${N_MEXFILES})
		list(GET MEXFILES ${counter} MEXFILE)
		list(GET MEXFILE_DEPENDENCIES ${counter} MEXFILE_DEPENDENCY)
		get_filename_component(MEXFILE_NAME "${MEXFILE}" NAME_WE)
		matlab_add_mex(NAME ${MEXFILE_NAME}Matlab SRC ${MEXFILE} convertGraph.cpp LINK_TO ${MEXFILE_DEPENDENCY} OUTPUT_NAME ${MEXFILE_NAME})
		#Set the outputs of the binaries to the right places
		foreach(BUILD_TYPE IN LISTS CMAKE_CONFIGURATION_TYPES)
			string(TOUPPER "${BUILD_TYPE}" UPPER_BUILD_TYPE)
			set_target_properties(${MEXFILE_NAME}Matlab PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${UPPER_BUILD_TYPE} ${CMAKE_BINARY_DIR}/matlabPackage/${BUILD_TYPE}/private PDB_OUTPUT_DIRECTORY_${UPPER_BUILD_TYPE} ${CMAKE_BINARY_DIR}/matlabPackage/${BUILD_TYPE}/private)
		endforeach()
	endforeach()
else()
	#On linux matlab ships with its own libstdc++, which we don't necessarily want to pull in. So we find our own libstd (the matlab one is set using LD_LIBRARY_PATH, so we won't pick it up here)
	find_library(LIBSTD_FOR_MEX stdc++)
	foreach(counter RANGE (${N_MEXFILES}-1))
		list(GET MEXFILES ${counter} MEXFILE)
		list(GET MEXFILE_DEPENDENCIES ${counter} MEXFILE_DEPENDENCY)
		get_filename_component(MEXFILE_NAME "${MEXFILE}" NAME_WE)
		matlab_add_mex(NAME ${MEXFILE_NAME}Matlab SRC ${MEXFILE} convertGraph.cpp LINK_TO ${MEXFILE_DEPENDENCY} OUTPUT_NAME ${MEXFILE_NAME})
	endforeach()
endif()
