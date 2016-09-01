#' @export
checkExactCounts <- function(object)
{
	errors <- c()
	graphClass <- class(object@graph)
	if(!(graphClass %in% c("igraph", "graphNEL", "graphAM")))
	{
		errors <- c(errors, "Slot graph must have class \"igraph\", \"graphNEL\" or \"graphAM\"")
	}
	if(length(errors) > 0) return(errors)
	return(TRUE)
}
setClass("exactCounts", slots = list(counts = "bigz", call = "call", start = "POSIXct", end = "POSIXct", graph = "ANY"), validity = checkExactCounts)
setMethod(f = "show", signature = "exactCounts", definition = function(object)
{
	cat("Number of connected subgraphs of a graph with ", length(object@counts)-1, " vertices, by number of subgraph vertices\n")
	result <- capture.output(object@counts)
	cat(tail(result, -1), sep="\n")
})
