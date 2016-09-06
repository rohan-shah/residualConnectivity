#' @include exactCounts-class.R
#' @include exactSpectra-class.R
setAs("exactCounts", "exactSpectra", def = function(from, to)
{
	binomialCoefficients <- chooseZ(length(from@counts)-1, 0:(length(from@counts)-1))
	return(new(to, spectra = as.bigq(from@counts, binomialCoefficients), graph = from@graph, call = from@call, start = from@start, end = from@end))
})
