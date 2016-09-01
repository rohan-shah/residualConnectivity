#' @export
conditionalMC <- function(probability, n, seed, graph)
{
	return(simpleMC("conditionalMC", graph=graph, probability=probability, n=n, seed=seed))
}
