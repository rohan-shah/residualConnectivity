countConnectedSubgraphs <- function(graph)
{
	if(class(graph) == "igraph")
	{
		result <- .Call("countConnectedSubgraphs_igraph", graph, PACKAGE="residualConnectivity")
	}
	else if(class(graph) == "graphNEL")
	{
		result <- .Call("countConnectedSubgraphs_graphNEL", graph, PACKAGE="residualConnectivity")
	}
	else if(class(graph) == "graphAM")
	{
		result <- .Call("countConnectedSubgraphs_graphAM", graph, PACKAGE="residualConnectivity")
	}
	else
	{
		stop("Input graph must have class \"igraph\", \"graphAM\" or \"graphNEL\"")
	}
	result@call <- match.call()
	result@graph <- graph
	return(result)
}
