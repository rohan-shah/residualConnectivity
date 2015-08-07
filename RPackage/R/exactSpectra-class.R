checkExactSpectra <- function(object)
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
setClass("exactSpectra", slots = list(spectra = "bigq", graph = "ANY"), validity = checkExactSpectra)
setMethod(f = "show", signature = "exactSpectra", definition = function(object)
{
	cat("Spectra of a graph with ", length(object@spectra)-1, " vertices, by number of subgraph vertices\n")
	result <- capture.output(as.double(object@spectra))
	cat(result, sep="\n")
})