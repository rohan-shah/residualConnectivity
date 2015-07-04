library(Rmpfr)
library(residualConnectivity)
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
for(size in 4:12)
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
		assign(variableName, new("exhaustiveSubgraphs", counts = toBigZ, call = quote(fff()), graph = graph.lattice(length = size, dim = 2)))
		save(list = variableName, file = rdataFile)
	}
}