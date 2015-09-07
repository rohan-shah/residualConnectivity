recursiveVarianceReduction <- function(probability, n, seed, graph)
{
	return(simpleMC("recursiveVarianceReduction", graph=graph, probability=probability, n=n, seed=seed))
}
