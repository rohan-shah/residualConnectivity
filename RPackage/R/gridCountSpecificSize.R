gridCountSpecificSize <- function(gridDimension, size, multithreaded)
{
	count <- .Call("gridCountSpecificSize", gridDimension, size, multithreaded, PACKAGE="residualConnectivity")
	return(mpfr(count))
}
