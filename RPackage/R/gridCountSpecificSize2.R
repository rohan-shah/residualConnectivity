#' @export
gridCountSpecificSize2 <- function(gridDimension, size)
{
	count <- .Call("gridCountSpecificSize2", gridDimension, size, PACKAGE="residualConnectivity")
	return(mpfr(count))
}
