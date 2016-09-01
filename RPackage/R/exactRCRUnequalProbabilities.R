#' @export
exactRCRUnequalProbabilities <- function(graph, probabilities)
{
	if(class(graph) == "igraph")
	{
		start <- Sys.time()
		result <- .Call("exactRCRUnequalProbabilities_igraph", graph, probabilities, PACKAGE="residualConnectivity")
		end <- Sys.time()
	}
	else if(class(graph) == "graphNEL")
	{
		start <- Sys.time()
		result <- .Call("exactRCRUnequalProbabilities_graphNEL", graph, probabilities, PACKAGE="residualConnectivity")
		end <- Sys.time()
	}
	else if(class(graph) == "graphAM")
	{
		start <- Sys.time()
		result <- .Call("exactRCRUnequalProbabilities_graphAM", graph, probabilities, PACKAGE="residualConnectivity")
		end <- Sys.time()
	}
	else
	{
		stop("Input graph must have class \"igraph\", \"graphAM\" or \"graphNEL\"")
	}
	return(mpfr(result))
}
