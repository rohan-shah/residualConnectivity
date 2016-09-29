#' @export
transferMatrixUnequalProbabilities <- function(probabilities)
{
	if(!is.matrix(probabilities))
	{
		stop("Input probabilities must be a matrix")
	}
	if(ncol(probabilities) != nrow(probabilities))
	{
		stop("Input probabilities must be a square matrix")
	}
	result <- .Call("transferMatrixUnequalProbabilities", probabilities, PACKAGE="residualConnectivity")
	return(mpfr(result))
}
