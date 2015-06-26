project(residualConnectivity)
file(COPY NAMESPACE DESTINATION ${CMAKE_BINARY_DIR}/RPackage)
file(COPY DESCRIPTION DESTINATION ${CMAKE_BINARY_DIR}/RPackage)

find_package(Rcpp REQUIRED)
add_subdirectory(R)
add_subdirectory(src)
#Copy Rcpp_exceptions to the R directory
get_property(RCPP_INCLUDE_DIRECTORIES TARGET Rcpp PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
foreach(RCPP_INCLUDE_DIRECTORY IN LISTS RCPP_INCLUDE_DIRECTORIES)
	if(EXISTS ${RCPP_INCLUDE_DIRECTORY}/Rcpp_exceptions.R)
		file(COPY ${RCPP_INCLUDE_DIRECTORY}/Rcpp_exceptions.R DESTINATION ${CMAKE_BINARY_DIR}/RPackage/R/)
	endif()
endforeach()

