exhaustiveSearch <- function(graph)
{
	if(class(graph) == "igraph")
	{
		result <- .Call("exhaustiveSearch_igraph", graph, PACKAGE="residualConnectivity")
	}
	else if(class(graph) == "graphNEL")
	{
		result <- .Call("exhaustiveSearch_graphNEL", graph, PACKAGE="residualConnectivity")
	}
	else if(class(graph) == "graphAM")
	{
		result <- .Call("exhaustiveSearch_graphAM", graph, PACKAGE="residualConnectivity")
	}
	else
	{
		stop("Input graph must have class \"igraph\", \"graphAM\" or \"graphNEL\"")
	}
	result@call <- match.call()
	result@graph <- graph
	return(result)
}
