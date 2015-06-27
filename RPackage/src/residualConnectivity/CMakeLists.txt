project(residualConnectivity)
cmake_minimum_required(VERSION 3.1)
add_library(residualConnectivityRPackage SHARED loadQt.cpp loadQt.h register.cpp crudeMC_igraph.cpp crudeMCInterfaces.h igraphInterface.h igraphInterface.cpp graphNELInterface.h graphNELInterface.cpp crudeMC_graphNEL.cpp crudeMC_graphAM.cpp graphAMInterface.h graphAMInterface.cpp)
target_link_libraries(residualConnectivityRPackage PUBLIC residualConnectivityCommon Rcpp crudeMCLib conditionalMCLib)
set_target_properties(residualConnectivityRPackage PROPERTIES OUTPUT_NAME "residualConnectivity")

if(BUILD_VISUALISATION)
	target_link_libraries(residualConnectivityRPackage PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets ${IGRAPH_LIBRARIES})
	if(WIN32)
		#Build types and corresponding QT build types
		set(BUILD_TYPES Release Debug RelWithDebInfo MinSizeRel)
		set(QT_BUILD_TYPES Release Debug Release Release)
		foreach(val RANGE 3)
			list(GET BUILD_TYPES ${val} BUILD_TYPE)
			list(GET QT_BUILD_TYPES ${val} QT_BUILD_TYPE)
			string(TOUPPER ${QT_BUILD_TYPE} QT_BUILD_TYPE_UPPER)

			target_compile_definitions(residualConnectivityRPackage PUBLIC _CRT_SECURE_NO_WARNINGS)
			get_target_property(QtCore_location Qt5::Core "LOCATION_${QT_BUILD_TYPE}")
			file(COPY ${QtCore_location} DESTINATION ${CMAKE_BINARY_DIR}/RPackage/${BUILD_TYPE}/src/)

			GET_FILENAME_COMPONENT(QtBinaryPath ${QtCore_location} PATH)
			if(EXISTS ${QtBinaryPath}/icuin51.dll)
				file(COPY ${QtBinaryPath}/icuin51.dll DESTINATION ${CMAKE_BINARY_DIR}/RPackage/${BUILD_TYPE}/src/)
				file(COPY ${QtBinaryPath}/icuuc51.dll DESTINATION ${CMAKE_BINARY_DIR}/RPackage/${BUILD_TYPE}/src/)
				file(COPY ${QtBinaryPath}/icudt51.dll DESTINATION ${CMAKE_BINARY_DIR}/RPackage/${BUILD_TYPE}/src/)
			endif()

			#copy boost serialization
			get_filename_component(NAME_PART ${Boost_SERIALIZATION_LIBRARY_${QT_BUILD_TYPE_UPPER}} NAME_WE)
			get_filename_component(PATH_PART ${Boost_SERIALIZATION_LIBRARY_${QT_BUILD_TYPE_UPPER}} PATH)
			file(COPY ${PATH_PART}/${NAME_PART}.dll DESTINATION ${CMAKE_BINARY_DIR}/RPackage/${BUILD_TYPE}/src/)

			get_target_property(QtWidgets_location Qt5::Widgets "LOCATION_${QT_BUILD_TYPE}")
			file(COPY "${QtWidgets_location}" DESTINATION ${CMAKE_BINARY_DIR}/RPackage/${BUILD_TYPE}/src/)

			get_target_property(QtGuiEgl_location Qt5::Gui_EGL "LOCATION_${QT_BUILD_TYPE}")
			file(COPY "${QtGuiEgl_location}" DESTINATION ${CMAKE_BINARY_DIR}/RPackage/${BUILD_TYPE}/src/)

			get_target_property(QtGuiGlesV2_location Qt5::Gui_GLESv2 "LOCATION_${QT_BUILD_TYPE}")
			file(COPY "${QtGuiGlesV2_location}" DESTINATION ${CMAKE_BINARY_DIR}/RPackage/${BUILD_TYPE}/src/)

			get_target_property(QtGui_location Qt5::Gui "LOCATION_${QT_BUILD_TYPE}")
			file(COPY "${QtGui_location}" DESTINATION ${CMAKE_BINARY_DIR}/RPackage/${BUILD_TYPE}/src/)
			target_link_libraries(residualConnectivityRPackage PRIVATE "${PROJECT_BINARY_DIR}/../../R.lib")

			#Now for platform stuff
			get_filename_component(QT_LIB_DIR "${QtGui_location}" DIRECTORY)
			if(EXISTS ${QT_LIB_DIR}/../plugins/platforms/qwindowsd.dll)
				file(COPY "${QT_LIB_DIR}/../plugins/platforms/qwindowsd.dll" DESTINATION "${CMAKE_BINARY_DIR}/RPackage/${BUILD_TYPE}/inst/libs/x64/platforms/")
			endif()
			if(EXISTS "${QT_LIB_DIR}/../plugins/platforms/qwindows.dll")
				file(COPY "${QT_LIB_DIR}/../plugins/platforms/qwindows.dll" DESTINATION "${CMAKE_BINARY_DIR}/RPackage/${BUILD_TYPE}/inst/libs/x64/platforms/")
			endif()

			#Now for MPFR
			get_target_property(MPFR_LOCATION mpfr "LOCATION_${QT_BUILD_TYPE}")
			file(COPY "${MPFR_LOCATION}" DESTINATION "${CMAKE_BINARY_DIR}/RPackage/${BUILD_TYPE}/src/")

			#Now for MPIR
			get_target_property(MPIR_LOCATION mpir "LOCATION_${QT_BUILD_TYPE}")
			file(COPY "${MPIR_LOCATION}" DESTINATION "${CMAKE_BINARY_DIR}/RPackage/${BUILD_TYPE}/src/")
		endforeach()
	endif()
endif()
if(WIN32)
	set_target_properties(residualConnectivityRPackage PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/../../../RPackage/Debug/src PDB_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/../../../RPackage/Debug/src)
	set_target_properties(residualConnectivityRPackage PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/../../../RPackage/Release/src PDB_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/../../../RPackage/Release/src)
else()
	set_target_properties(residualConnectivityRPackage PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/../../../RPackage/src)
endif()
