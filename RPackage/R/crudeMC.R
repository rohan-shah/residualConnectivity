#' @export
crudeMC <- function(probabilities, n, seed, graph)
{
	if(missing(graph))
	{
		stop("Input graph cannot be missing")
	}
	if(missing(probabilities))
	{
		stop("Input probabilities cannot be missing")
	}
	if(missing(n))
	{
		stop("Input n cannot be missing")
	}
	if(missing(seed))
	{
		stop("Input seed cannot be missing")
	}
	if(length(n) != 1 || mode(n) != "numeric")
	{
		stop("Input `n' must be a single number")
	}
	if(abs(n - round(n)) > 1e-3)
	{
		stop("Input `n' must be an integer")
	}
	if(mode(probabilities) != "numeric")
	{
		stop("Input `probabilities' must be a numeric vector")
	}
	if(any(probabilities < 0 | probabilities > 1))
	{
		stop("Input `probabilities' must be between 0 and 1")
	}
	if(class(graph) %in% c("igraph", "graphNEL", "graphAM"))
	{
		start <- Sys.time()
		estimate <- .Call("crudeMC", graph, probabilities, n, seed, PACKAGE="residualConnectivity")
		end <- Sys.time()
	}
	else 
	{
		stop("Input graph must have class \"igraph\", \"graphAM\" or \"graphNEL\"")
	}
	call <- match.call()
	return(new("monteCarloResult", start = start, end = end, call = call, estimate = estimate))
}
