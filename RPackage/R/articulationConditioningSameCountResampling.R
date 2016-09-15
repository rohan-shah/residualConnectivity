#' @export
articulationConditioningSameCountResampling <- function(probabilities, n, seed, graph, initialRadius, finalStepSampleSize, verbose=FALSE)
{
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
	if(missing(graph))
	{
		stop("Input graph cannot be missing")
	}
	if(missing(initialRadius))
	{
		stop("Input initialRadius cannot be missing")
	}
	if(missing(finalStepSampleSize))
	{
		stop("Input finalStepSampleSize cannot be missing")
	}
	if(any(probabilities < 0 | probabilities > 1 || is.na(probabilities)))
	{
		stop("Input probabilities must be a single number")
	}
	if(length(n) != 1 || n < 1 || is.na(n))
	{
		stop("Input n must be a single integer")
	}
	if(length(initialRadius) != 1 || initialRadius < 0 || is.na(initialRadius))
	{
		stop("Input initialRadius must be a single integer")
	}
	if(abs(n - as.integer(n)) > 1e-5)
	{
		stop("Input n must be an integer")
	}
	if(length(seed) != 1 || seed < 1 || is.na(seed))
	{
		stop("Input seed must be a single integer")
	}
	if(abs(seed - as.integer(seed)) > 1e-5)
	{
		stop("Input seed must be a single integer")
	}
	if(finalStepSampleSize < 1)
	{
		stop("Input finalStepSampleSize must be at least 1")
	}
	if(class(graph) %in% c("igraph", "graphNEL", "graphAM"))
	{
		start <- Sys.time()
		estimate <- .Call("articulationConditioningSameCountResampling", graph, probabilities, n, initialRadius, seed, finalStepSampleSize, verbose, PACKAGE="residualConnectivity")
		end <- Sys.time()
	}
	else
	{
		stop("Input graph must have class \"igraph\", \"graphAM\" or \"graphNEL\"")
	}
	return(new("monteCarloResult", start = start, end = end, call = match.call(), estimate = estimate))
}
