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
	else if(class(graph) == "graphNEL")
	{
		vertexCoordinates <- NULL
		if(length(graph@renderInfo@nodes) > 0)
		{
			vertexCoordinates <- c(graph@renderInfo@nodes$nodeX, graph@renderInfo@nodes$nodeY)
		}
		else
		{
			loadResult <- try(library(Rgraphviz), silent = TRUE)
			if(class(loadResult) == "try-error")
			{
				warning("The input format does not support vertex positions, arbitrary positions inserted.")
			}
			else
			{
				graph <- layoutGraph(graph, layoutType="dot")
				vertexCoordinates <- c(graph@renderInfo@nodes$nodeX, graph@renderInfo@nodes$nodeY)
			}
		}
		result <- .Call("crudeMC_graphNEL", graph, vertexCoordinates, probability, n, seed, PACKAGE="residualConnectivity")
		return (result / n)
	}
	else if(class(graph) == "graphAM")
	{
		vertexCoordinates <- NULL
		if(length(graph@renderInfo@nodes) > 0)
		{
			vertexCoordinates <- c(graph@renderInfo@nodes$nodeX, graph@renderInfo@nodes$nodeY)
		}
		else
		{
			loadResult <- try(library(Rgraphviz), silent = TRUE)
			if(class(loadResult) == "try-error")
			{
				warning("The input format does not support vertex positions, arbitrary positions inserted.")
			}
			else
			{
				graph <- layoutGraph(graph, layoutType="dot")
				vertexCoordinates <- c(graph@renderInfo@nodes$nodeX, graph@renderInfo@nodes$nodeY)
			}
		}
		result <- .Call("crudeMC_graphAM", graph, vertexCoordinates, probability, n, seed, PACKAGE="residualConnectivity")
		return (result / n)
	}
}