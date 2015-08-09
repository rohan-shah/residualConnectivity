spectraToRCR <- function(input, probability)
{
	if(!(class(input) %in% c("estimatedCounts", "estimatedSpectra")))
	{
		stop("First argument must have class estimatedCounts or estimatedSpectra")
	}
	if(length(probability) != 1 || probability < 0 || probability > 1 || (!is.numeric(probability) && class(probability) != "mpfr"))
	{
		stop("Input probability must be a single number between 0 and 1")
	}
	if(is.numeric(probability))
	{
		prec <- max(getPrec(input@data))
	}
	else
	{
		prec <- getPrec(probability)
	}
	probabilityComplement <- 1 - probability
	result <- mpfr(0, prec)
	nVertices <- length(input@data)-1
	if(class(input) == "estimatedCounts")
	{
		for(i in 0:nVertices)
		{
			result <- result + input@data[i+1] * (probability ^ i) * (probabilityComplement ^(nVertices - i))
		}
	}
	else if(class(input) == "estimatedSpectra")
	{
		binomialCoefficients <- chooseZ(nVertices, 0:nVertices)
		for(i in 0:nVertices)
		{
			result <- result + binomialCoefficients[i+1] * input@data[i+1] * (probability ^ i) * (probabilityComplement ^(nVertices - i))
		}
		#Because we're multiplying by large integers, apparently the number of bits of precision changes as we go through the loop
		result <- roundMpfr(result, prec)
	}
	else stop("Internal error")
	return(result)
}