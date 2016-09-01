#' @export
stochasticEnumeration <- function(graph, budget, form = "spectra", seed = 1, optimized = TRUE, nPermutations = 1, outputStatus = FALSE)
{
	if(form == "spectra") counts <- FALSE
	else if(form == "counts") counts <- TRUE
	else stop("Input form must have value \"counts\" or \"spectra\"")
	if(class(graph) == "igraph")
	{
		if(igraph::is.directed(graph))
		{
			stop("Input `graph' must be undirected")
		}
		start <- Sys.time()
		result <- .Call("stochasticEnumeration_igraph", graph, optimized, budget, seed, counts, nPermutations, outputStatus, PACKAGE="residualConnectivity")
		end <- Sys.time()
	}
	else if(class(graph) == "graphNEL")
	{
		start <- Sys.time()
		result <- .Call("stochasticEnumeration_graphNEL", graph, optimized, budget, seed, counts, nPermutations, outputStatus, PACKAGE="residualConnectivity")
		end <- Sys.time()
	}
	else if(class(graph) == "graphAM")
	{
		start <- Sys.time()
		result <- .Call("stochasticEnumeration_graphAM", graph, optimized, budget, seed, counts, nPermutations, outputStatus, PACKAGE="residualConnectivity")
		end <- Sys.time()
	}
	else 
	{
		stop("Input graph must have class \"igraph\", \"graphAM\" or \"graphNEL\"")
	}
	if(counts)
	{
		result <- new("estimatedCounts", data = result, call = match.call(), start = start, end = end)
	}
	else 
	{
		result <- new("estimatedSpectra", data = result, call = match.call(), start = start, end = end)
	}
	return(result)
}
setClass("estimatedCounts", slots = list(data = "mpfr", call = "call", start = "POSIXct", end = "POSIXct"))
setClass("estimatedSpectra", slots = list(data = "mpfr", call = "call", start = "POSIXct", end = "POSIXct"))
