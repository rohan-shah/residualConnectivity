spectraToRCR <- function(data, probability)
{
	if(missing(data)) stop("Input data is required")
	if(missing(probability)) stop("Input probability is required")
	
	if(!(class(data) %in% c("estimatedCounts", "estimatedSpectra")))
	{
		stop("Input data must have class estimatedCounts or estimatedSpectra")
	}
	if(any(probability < 0) || any(probability > 1) || (!is.numeric(probability) && class(probability) != "mpfr"))
	{
		stop("Input probability must be numbers between 0 and 1")
	}
	if(is.numeric(probability))
	{
		prec <- max(getPrec(data@data))
	}
	else
	{
		prec <- getPrec(probability)
	}
	probabilityComplement <- 1 - probability
	result <- mpfr(0, prec)
	nVertices <- length(data@data)-1
	if(class(data) == "estimatedCounts")
	{
		for(i in 0:nVertices)
		{
			result <- result + data@data[i+1] * (probability ^ i) * (probabilityComplement ^(nVertices - i))
		}
	}
	else if(class(data) == "estimatedSpectra")
	{
		binomialCoefficients <- chooseZ(nVertices, 0:nVertices)
		for(i in 0:nVertices)
		{
			result <- result + binomialCoefficients[i+1] * data@data[i+1] * (probability ^ i) * (probabilityComplement ^(nVertices - i))
		}
		#Because we're multiplying by large integers, apparently the number of bits of precision changes as we go through the loop
		result <- roundMpfr(result, prec)
	}
	else stop("Internal error")
	return(result)
}
spectraToRCRList <- function(data, probability)
{
	if(missing(data)) stop("Input data is required")
	if(missing(probability)) stop("Input probability is required")
	if(!is.list(data)) stop("Input data must be a list")
	classes <- unlist(lapply(data, class))
	uniqueClasses <- unique(classes)
	
	if(length(uniqueClasses) != 1 || !(uniqueClasses %in% c("estimatedCounts", "estimatedSpectra")))
	{
		stop("Input data must be a list of objects of one class (either estimatedCounts or estimatedSpectra)")
	}
	if(any(probability < 0) || any(probability > 1) || (!is.numeric(probability) && class(probability) != "mpfr"))
	{
		stop("Input probability must be numbers between 0 and 1")
	}
	if(is.numeric(probability))
	{
		prec <- max(unlist(lapply(data, function(x) getPrec(x@data))))
	}
	else
	{
		prec <- getPrec(probability)
	}
	result <- mpfr(rep(0, length(data)), prec)

	nVertices <- unlist(lapply(data, function(x) length(x@data)-1))
	if(length(unique(nVertices)) != 1) stop("All input data objects must have the same number of vertices")
	nVertices <- unique(nVertices)

	columns <- lapply(0:nVertices, function(x) do.call(c, lapply(data, function(y) y@data[x+1])))
	resultMatrix <- mpfrArray(0, dim = c(length(probabilities), length(data)), precBits = prec)
	rownames(resultMatrix) <- as.character(probability)
	if(uniqueClasses == "estimatedCounts")
	{
		for(currentProbabilityIndex in 1:length(probability))
		{
			result <- 0
			currentProbability <- probability[currentProbabilityIndex]
			probabilityComplement <- 1 - currentProbability
			for(i in 0:nVertices)
			{
				result <- result + columns[[i+1]] * (currentProbability ^ i) * (probabilityComplement^(nVertices - i))
			}
			resultMatrix[currentProbabilityIndex,] <- result
		}
	}
	else if(uniqueClasses == "estimatedSpectra")
	{
		binomialCoefficients <- Rmpfr:::.bigz2mpfr(chooseZ(nVertices, 0:nVertices))
		for(currentProbabilityIndex in 1:length(probability))
		{
			result <- 0
			currentProbability <- probability[currentProbabilityIndex]
			probabilityComplement <- 1 - currentProbability
			for(i in 0:nVertices)
			{
				result <- result + binomialCoefficients[i+1] * columns[[i+1]] * (currentProbability ^ i) * (probabilityComplement ^ (nVertices - i))
			}
			resultMatrix[currentProbabilityIndex,] <- result
		}
		#Because we're multiplying by large integers, apparently the number of bits of precision changes as we go through the loop
		resultMatrix <- roundMpfr(resultMatrix, prec)
	}
	else stop("Internal error")
	return(resultMatrix)
}
