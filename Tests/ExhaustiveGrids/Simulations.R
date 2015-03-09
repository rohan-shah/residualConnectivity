wd <- getwd()
setwd("../../x64/Release")
probabilities <- c(seq(0.01, 0.09, 0.01), seq(0.1, 0.7, 0.1), 0.75, seq(0.8, 0.9, 0.01))

getTruth <- function(gridDimension, probabilities)
{
	output <- shell(paste0(file.path(getwd(), "ExhaustiveProbability.exe"), " --gridGraph ", gridDimension, " --probability ", do.call(paste, c(as.list(probabilities), sep=" ")), " < ", file.path(wd, "..", paste0("Exhaustive", gridDimension, ".txt"))), intern=TRUE)
	output <- tail(output, 1)
	output <- substr(output, 17, nchar(output))
	text <- textConnection(output)
	result <- unlist(read.csv(file=text, header=FALSE))
	close(text)
	names(result) <- NULL
	return(result)
}
getCrudeMC <- function(gridDimension, probability, n, seed)
{
	output <- system(paste0("crudeMC.exe --gridGraph ", gridDimension, " --probability ", probability, " --n ", n, " --seed ", seed), show.output.on.console=FALSE, intern=TRUE)
	nSingleComponent <- as.numeric(strsplit(output, "/")[[1]][1]) / n
	return(nSingleComponent)
}
getSplitting <- function(gridDimension, probability, initialRadius, nGraphs, splittingFactor, seed)
{
	output <- system(paste0("splitting.exe --gridGraph ", gridDimension, " --probability ", probability, " --nGraphs ", nGraphs, " --seed ", seed, " --initialRadius ", initialRadius, " --splittingFactor ", do.call(paste, as.list(splittingFactor))), show.output.on.console=FALSE, intern=TRUE)
	output <- tail(output, 1)
	if(substr(output, 1, 26) != "Estimated probability was ") browser()
	result <- as.numeric(substr(output, 27, nchar(output)))
	return(result)
}
run <- function(probabilities, splittingSteps, gridDimension, splittingFactors)
{
	truth <- vector(mode="numeric", length= length(probabilities))
	for(i in 1:length(probabilities)) truth[i] <- getTruth(gridDimension, probabilities[[i]])

	crudeMC <- numeric(length(probabilities))
	splitting <- list()
	for(splittingStep in splittingSteps) splitting[[as.character(splittingStep)]] <- numeric(length(probabilities))
	for(counter in 1:length(probabilities))
	{
		crudeMC[counter] <- getCrudeMC(gridDimension, probabilities[[counter]], n = 10000, seed = 1)
		estimate <- truth[counter]
		for(splittingStep in splittingSteps) 
		{
			if(is.null(splittingFactors))
			{
				splittingFactor <- (1/estimate)^(1/splittingStep)
			}
			else
			{
				splittingFactor <- splittingFactors[[counter]]
			}
			splitting[[as.character(splittingStep)]][counter] <- getSplitting(gridDimension, probabilities[[counter]], initialRadius = splittingStep, nGraphs = 10000, splittingFactor = splittingFactor, seed = 1)
		}
	}	
	return(list(truth=truth, crudeMC = crudeMC, splitting = splitting, gridDimension = gridDimension))
}
doPlots <- function(run)
{
	nSplittingLevels <- length(run$splitting)
	colours <- rainbow(2 + nSplittingLevels)
	
	if("truth" %in% names(run))
	{
		splittingBiases <- list()
		for(j in 1:nSplittingLevels) splittingBiases[[j]] <- run$splitting[[j]] - run$truth
		plot(probabilities, run$crudeMC - run$truth, col = colours[1], type = "l", ylim = range(do.call(c, c(splittingBiases, run$crudeMC - run$truth))))
		for(j in 1:nSplittingLevels) lines(probabilities, splittingBiases[[j]], col = colours[1 + j])
		legend(0.0, -0.02, legend = c("crude MC bias", paste0("splitting", 1:nSplittingLevels, " bias")), fill = colours[1:(length(colours)-1)])
	}
	
	if("truth" %in% names(run))
	{
		plot(probabilities, run$truth, type="l", col = colours[1])
		lines(probabilities, run$crudeMC, col = colours[2])
		for(j in 1:nSplittingLevels) lines(probabilities, run$splitting[[j]], col = colours[2 + j])
		legend(0.0, 0.8, legend = c("truth", "crude MC", paste0("splitting", 1:nSplittingLevels)), fill = colours)
	}
	else
	{
		plot(probabilities, run$crudeMC, type = "l", col = colours[1])
		for(j in 1:nSplittingLevels) lines(probabilities, run$splitting[[j]], col = colours[1 + j])
		legend(0.0, 0.8, legend = c("crude MC", paste0("splitting", 1:nSplittingLevels)), fill = colours[1:(length(colours)-1)])
	}
}

splittingSteps <- list(1, 1:2, 1:3, 4, 5, 4)
splittingFactors <- list()
#For number 6 it's not good enough to split equally at every level, we have to be smarter. 
splittingFactors[[6]] <- vector(mode="list", length = length(probabilities))
values <- rbind(
	c(0, 0.09, 1.2, 1.8, 1.5, 10), 
	c(0.1, 0.1, 1.2, 1.8, 2.2, 2.3),
	c(0.2, 0.2, 1.1, 1.8, 4, 13),
	c(0.3, 0.3, 1.1, 1.3, 3, 11),
	c(0.4, 0.4, 1.1, 1.1, 2.1, 5.5),
	c(0.5, 0.5, 1.1, 1.1, 1.8, 2.2), 
	c(0.6, 0.6, 1.1, 1.1, 1.3, 1.6), 
	c(0.7, 0.7, 1.1, 1.1, 1.3, 1),
	c(0.75, 0.75, 1, 1.1, 1.1, 1.1),
	c(0.8, 0.9, 1.1, 1.1, 1.1, 1.1)
)
for(i in 1:nrow(values))
{
	for(k in which(probabilities >= values[i, 1]-0.001 & probabilities <= values[i, 2]+0.001)) splittingFactors[[6]][[k]] <- values[i, 3:6]
}
gridDimensions <- as.list(2:7)
runs <- list()
for(runCounter in 1:6)
{
	file <- file.path(wd, paste0("gridDimension", gridDimensions[[runCounter]], ".pdf"))
	if(!file.exists(file))
	{
		runs[[runCounter]] <- run(probabilities, gridDimension = gridDimensions[[runCounter]], splittingSteps=splittingSteps[[runCounter]], splittingFactors= splittingFactors[[runCounter]])
		pdf(file)
		doPlots(runs[[runCounter]])
		dev.off()
	}
	cat("Finished gridDimension = ", gridDimensions[[runCounter]], "\n")
}
setwd(wd)