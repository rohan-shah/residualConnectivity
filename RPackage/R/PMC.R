PMC <- function(graph, n, seed = 1)
{
	if(missing(graph)) stop("Input graph cannot be missing")
	if(missing(n)) stop("Input n cannot be missing")

	if(length(n) != 1 || !is.numeric(n)) stop("Input n must be a single number")
	if(abs(n - round(n)) > 1e-3) stop("Input n must be an integer")
	n <- as.integer(n)

	if(length(seed) != 1 || !is.numeric(seed)) stop("Input seed must be a single number")
	if(abs(seed - round(seed)) > 1e-3) stop("Input seed must be an integer")
	seed <- as.integer(seed)

	if(class(graph) == "igraph")
	{
		if(is.directed(graph))
		{
			stop("Input `graph' must be undirected")
		}
		result <- .Call("PMC_igraph", graph, n, seed, PACKAGE="residualConnectivity")
	}
	else if(class(graph) == "graphNEL")
	{
		result <- .Call("PMC_graphNEL", graph, n, seed, PACKAGE="residualConnectivity")
	}
	else if(class(graph) == "graphAM")
	{
		result <- .Call("PMC_graphAM", graph, n, seed, PACKAGE="residualConnectivity")
	}
	else 
	{
		stop("Input graph must have class \"igraph\", \"graphAM\" or \"graphNEL\"")
	}
	estimatedSpectra <- new("estimatedSpectra", data = result)
	return(estimatedSpectra)
}