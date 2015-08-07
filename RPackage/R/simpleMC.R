simpleMC <- function(functionName, graph, probability, n, seed=1)
{
	if(length(functionName) != 1)
	{
		stop("Input functionName to simpleMC must have length 1") 
	}
	if(mode(functionName) != "character")
	{
		stop("Input functionName must be a string")
	}
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
		if(is.directed(graph))
		{
			stop("Input `graph' must be undirected")
		}
		vertexCoordinates <- layout.auto(graph)
		result <- .Call(paste0(functionName, "_igraph"), graph, vertexCoordinates, probability, n, seed, PACKAGE="residualConnectivity")
		return(result)
	}
	else if(class(graph) == "graphNEL")
	{
		vertexCoordinates <- NULL
		if(length(graph@renderInfo@nodes) > 0)
		{
			vertexCoordinates <- cbind(graph@renderInfo@nodes$nodeX, graph@renderInfo@nodes$nodeY)
		}
		result <- .Call(paste0(functionName, "_graphNEL"), graph, vertexCoordinates, probability, n, seed, PACKAGE="residualConnectivity")
		return (result)
	}
	else if(class(graph) == "graphAM")
	{
		vertexCoordinates <- NULL
		if(length(graph@renderInfo@nodes) > 0)
		{
			vertexCoordinates <- cbind(graph@renderInfo@nodes$nodeX, graph@renderInfo@nodes$nodeY)
		}
		result <- .Call(paste0(functionName, "_graphAM"), graph, vertexCoordinates, probability, n, seed, PACKAGE="residualConnectivity")
		return (result)
	}
	else 
	{
		stop("Input graph must have class \"igraph\", \"graphAM\" or \"graphNEL\"")
	}
}
