#' @export
transferMatrixCountConnectedSubgraphsOfSquareGrid <- function(size)
{
	if(size < 2 || size > 12)
	{
		stop("Dimension must be betwene 2 and 12")
	}
	startTime <- Sys.time()
	results <- .Call("transferMatrixIdentical", size, PACKAGE="residualConnectivity")
	endTime <- Sys.time()
	resultsBigZ <- as.bigz(results)
	return(new("exactCounts", counts = resultsBigZ, call = match.call(), graph = graph.lattice(length = size, dim = 2), start = startTime, end = endTime))
}
