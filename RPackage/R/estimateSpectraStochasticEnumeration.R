estimateSpectraStochasticEnumeration <- function(graph, budget, form = "spectra", seed = 1)
{
	if(form == "spectra") counts <- FALSE
	else if(form == "counts") counts <- TRUE
	else stop("Input form must have value \"counts\" or \"spectra\"")
	if(class(graph) == "igraph")
	{
		if(is.directed(graph))
		{
			stop("Input `graph' must be undirected")
		}
		result <- .Call("stochasticEnumeration_igraph", graph, budget, seed, counts, PACKAGE="residualConnectivity")
	}
	else if(class(graph) == "graphNEL")
	{
		result <- .Call("stochasticEnumeration_graphNEL", graph, budget, seed, counts, PACKAGE="residualConnectivity")
	}
	else if(class(graph) == "graphAM")
	{
		result <- .Call("stochasticEnumeration_graphAM", graph, budget, seed, counts, PACKAGE="residualConnectivity")
	}
	else 
	{
		stop("Input graph must have class \"igraph\", \"graphAM\" or \"graphNEL\"")
	}
	if(counts)
	{
		result <- new("estimatedCounts", data = result)
	}
	else 
	{
		result <- new("estimatedSpectra", data = result)
	}
	return(result)
}
setClass("estimatedCounts", slots = list(data = "mpfr"))
setClass("estimatedSpectra", slots = list(data = "mpfr"))