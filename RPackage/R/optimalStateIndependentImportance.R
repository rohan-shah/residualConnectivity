optimalStateIndependentImportance <- function(exact, probabilities)
{
	if(!is(exact, "exactCounts"))
	{
		stop("Input exact must be an object of class exactCounts")
	}
	bestImportance <- .Call("optimalStateIndependentImportance", exact, probabilities, PACKAGE="residualConnectivity")
	result <- data.frame(probability = probabilities, bestImportance = bestImportance)
	return(result)
}
