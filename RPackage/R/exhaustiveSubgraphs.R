checkExhaustiveSubgraphs <- function(object)
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
setClass("exhaustiveSubgraphs", slots = list(counts = "bigz", call = "call", graph = "ANY"), validity = checkExhaustiveSubgraphs)
setMethod(f = "show", signature = "exhaustiveSubgraphs", definition = function(object)
{
	cat("Exhaustive subgraphs count of graph with ", length(object@counts)-1, " vertices\n")
	result <- capture.output(object@counts)
	cat(tail(result, -1), sep="\n")
})

