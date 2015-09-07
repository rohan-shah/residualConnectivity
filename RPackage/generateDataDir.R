library(Rmpfr)
hasPackage <- require(residualConnectivity)
library(R.utils)
if(!hasPackage) q(save="no")
library(igraph)
library(stringr)
fff <- function()
{
	browser()
}
if(!file.exists("data"))
{
	dir.create("data")
}
NAtime <- as.numeric(NA)
class(NAtime) <- "POSIXct"
for(size in 3:12)
{
	rdataFile <- file.path("data", paste0("grid", size, "Counts.RData"))
	if(!file.exists(rdataFile))
	{
		textFile <- file.path("..", "Tests", paste0("Exhaustive", size, ".txt"))
		contents <- readLines(textFile)
		contents <- tail(contents, -1)
		index <- str_locate(contents, ":")
		toBigZ <- as.bigz(substr(contents, index+1, nchar(contents)))

		variableName <- paste0("grid", size, "Counts")
		assign(variableName, new("exactCounts", counts = toBigZ, call = quote(fff()), graph = graph.lattice(length = size, dim = 2), start = NAtime, end = NAtime))

		#If we're creating a data file that didn't already exist, we need to trigger CMake to run again. 
		if(!file.exists(rdataFile)) touchFile("CMakeLists.txt")
		save(list = variableName, file = rdataFile)
	}
}

textFile <- file.path("..", "Tests", "Dodecahedron.txt")
contents <- readLines(textFile)
contents <- tail(contents, -1)
index <- str_locate(contents, ":")
toBigZ <- as.bigz(substr(contents, index+1, nchar(contents)))

dodecahedronGraph <- read.graph("data/dodecahedron.graphml", format = "graphml")
dodecahedronCounts <- new("exactCounts", counts = toBigZ, call = quote(fff()), graph = dodecahedronGraph, start = NAtime, end = NAtime)
save(dodecahedronCounts, file = file.path("data", "dodecahedronCounts.RData"))
