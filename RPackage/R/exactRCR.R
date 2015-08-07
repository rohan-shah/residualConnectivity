exactRCR <- function(counts, probability)
{
	if(class(counts) != "exactCounts")
	{
		stop("Input counts must have class exactCounts")
	}
	if(length(probability) != 1 || probability < 0 || probability > 1 || (!is.numeric(probability) && class(probability) != "mpfr"))
	{
		stop("Input probability must be a single number between 0 and 1")
	}
	if(is.numeric(probability))
	{
		prec <- 50
	}
	else
	{
		prec <- getPrec(probability)
	}
	nVertices <- length(counts@counts)-1
	probabilityComplement <- 1 - probability
	result <- mpfr(0, prec)
	for(i in 0:nVertices)
	{
		result <- result + mpfr(counts@counts[i+1], prec) * (probability ^ i) * (probabilityComplement ^(nVertices - i))
	}
	return(result)
}
