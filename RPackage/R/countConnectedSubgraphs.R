countConnectedSubgraphs <- function(graph)
{
	if(class(graph) == "igraph")
	{
		start <- Sys.time()
		result <- .Call("countConnectedSubgraphs_igraph", graph, PACKAGE="residualConnectivity")
		end <- Sys.time()
	}
	else if(class(graph) == "graphNEL")
	{
		start <- Sys.time()
		result <- .Call("countConnectedSubgraphs_graphNEL", graph, PACKAGE="residualConnectivity")
		end <- Sys.time()
	}
	else if(class(graph) == "graphAM")
	{
		start <- Sys.time()
		result <- .Call("countConnectedSubgraphs_graphAM", graph, PACKAGE="residualConnectivity")
		end <- Sys.time()
	}
	else
	{
		stop("Input graph must have class \"igraph\", \"graphAM\" or \"graphNEL\"")
	}
	result@call <- match.call()
	result@start <- start
	result@end <- end
	result@graph <- graph
	return(result)
}
