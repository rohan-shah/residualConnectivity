crudeMC <- function(graph, probability, n, seed=1)
{
	if(length(n) != 1 || mode(n) != "numeric")
	{
		stop("Input `n' must be a single number")
	}
	if(abs(n - round(n)) > 1e-3)
	{
		stop("Input `n' must be an integer")
	}
	if(length(probability) != 1 || mode(n) != "numeric")
	{
		stop("Input `probability' must be a single number")
	}
	if(probability < 0 || probability > 1)
	{
		stop("Input `probability' must be between 0 and 1")
	}
	if(class(graph) == "igraph")
	{
		vertexCoordinates <- layout.auto(graph)
		result <- .Call("crudeMC_igraph", graph, vertexCoordinates, probability, n, seed, PACKAGE="residualConnectivity")
		return(result / n)
	}
}