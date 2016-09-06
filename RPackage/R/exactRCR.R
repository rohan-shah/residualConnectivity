#' @export 
exactRCR <- function(input, probability)
{
	if(!(class(input) %in% c("exactCounts", "exactSpectra")))
	{
		stop("First argument must have class exactCounts or exactSpectra")
	}
	if(length(probability) != 1)
	{
		stop("Input probability must have length 1")
	}
	if(any(probability < 0) || any(probability > 1) || (!is.numeric(probability) && class(probability) != "mpfr"))
	{
		stop("Input probability must be numbers between 0 and 1")
	}
	if(is.numeric(probability))
	{
		prec <- 50
	}
	else
	{
		prec <- getPrec(probability)
	}
	probabilityComplement <- 1 - probability
	result <- mpfr(0, prec)
	if(class(input) == "exactCounts")
	{
		nVertices <- length(input@counts)-1
		for(i in 0:nVertices)
		{
			result <- result + mpfr(input@counts[i+1], prec) * (probability ^ i) * (probabilityComplement ^(nVertices - i))
		}
	}
	else if(class(input) == "exactSpectra")
	{
		nVertices <- length(input@spectra)-1
		binomialCoefficients <- chooseZ(nVertices, 0:nVertices)
		for(i in 0:nVertices)
		{
			result <- result + binomialCoefficients[i+1] * mpfr(input@spectra[i+1], prec) * (probability ^ i) * (probabilityComplement ^(nVertices - i))
		}
		#Because we're multiplying by large integers, apparently the number of bits of precision changes as we go through the loop
		result <- roundMpfr(result, prec)
	}
	else stop("Internal error")
	return(result)
}
