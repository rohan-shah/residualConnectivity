library(Rmpfr)
library(R.utils)
hasPackage <- require(residualConnectivity)
if(!hasPackage) 
{
	q(save="no")
}
library(igraph)
library(stringr)
#Make a data directory
if(!file.exists("data"))
{
	dir.create("data")
}
dodecahedronGraphMLPath <- file.path(path.package("residualConnectivity"), "data", "dodecahedron.graphml")
dodecahedron <- read.graph(file = dodecahedronGraphMLPath, format = "graphml")
dodecahedronAdjacency <- get.adjacency(dodecahedron)
library(R.matlab)

outputFile <- file.path("data", "dodecahedron.mat")
if(!file.exists(outputFile)) touchFile("./CMakeLists.txt")
writeMat(outputFile, dodecahedron = dodecahedronAdjacency)