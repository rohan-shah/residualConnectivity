project(residualConnectivity)
file(GLOB RFILES *.R)
file(COPY ${RFILES} DESTINATION ${CMAKE_BINARY_DIR}/RPackage/R)
