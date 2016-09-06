#' @export 
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

	if(class(graph) %in% c("igraph", "graphNEL", "graphAM"))
	{
		start <- Sys.time()
		result <- .Call("PMC", graph, n, seed, PACKAGE="residualConnectivity")
		end <- Sys.time()
	}
	else 
	{
		stop("Input graph must have class \"igraph\", \"graphAM\" or \"graphNEL\"")
	}
	estimatedSpectra <- new("estimatedSpectra", data = result, call = match.call(), start = start, end = end)
	return(estimatedSpectra)
}
