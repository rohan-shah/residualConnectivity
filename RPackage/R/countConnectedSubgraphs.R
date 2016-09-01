#' @export
countConnectedSubgraphs <- function(graph)
{
	if(class(graph) %in% c("igraph", "graphNEL", "graphAM"))
	{
		start <- Sys.time()
		result <- .Call("countConnectedSubgraphs", graph, PACKAGE="residualConnectivity")
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
