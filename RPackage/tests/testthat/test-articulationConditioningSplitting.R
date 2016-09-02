context("check articulationConditioningSplitting function")
library(graph)

test_that("Function can be called using graphNEL, graphAM and igraph, and gives same results", 
	{
		gridSizes <- 4:5
		probabilities <- c(0.3, 0.6)
		for(gridSize in gridSizes)
		{
			igraph <- igraph::graph.lattice(dim = 2, length=gridSize)
			graphNEL <- igraph::igraph.to.graphNEL(igraph)
			graphAM <- as(graphNEL, "graphAM")
			for(probability in probabilities)
			{

				capture.output(result_igraph <- articulationConditioningSplitting(graph=igraph, n = 1000, seed = gridSize, initialRadius = 3, probabilities = probability, splittingFactors = rep(1L, 3)))
				capture.output(result_graphNEL <- articulationConditioningSplitting(graph=graphNEL, n = 1000, seed = gridSize, initialRadius = 3, probabilities = probability, splittingFactors = rep(1L, 3)))
				capture.output(result_graphAM <- articulationConditioningSplitting(graph=graphAM, n = 1000, seed = gridSize, initialRadius = 3, probabilities = probability, splittingFactors = rep(1L, 3)))
				expect_identical(result_igraph@estimate, result_graphNEL@estimate)
				expect_identical(result_graphNEL@estimate, result_graphAM@estimate)
			}
		}
	})
test_that("Values agree with exact values for small grids and uniform probabilities",
{
	probabilities <- seq(0.1, 0.9, length.out = 9)
	for(gridSize in 3:5)
	{
		graph <- igraph::graph.lattice(dim = 2, length=gridSize)
		data(list = paste0("grid", gridSize, "Counts"), package="residualConnectivity")
		subgraphCounts <- get(paste0("grid", gridSize, "Counts"))
		for(probability in probabilities)
		{
			capture.output(methodResult <- articulationConditioningSplitting(graph=graph, n = 20000, seed = gridSize, probabilities = probability, initialRadius = 3, splittingFactors = rep(1L, 3)))
			exactResult <- exactRCR(subgraphCounts, probability)
			expect_equal(as.numeric(methodResult@estimate), as.numeric(exactResult), tolerance = 3e-2)
		}
	}
})
test_that("Values agree with exact values for small grids and non-uniform probabilities",
{
	graph <- igraph::graph.lattice(dim = 2, length=4)
	probabilities <- rbind(c(0.8, 0.2, 0.8), c(0.3, 0.6, 0.9))
	for(index in 1:nrow(probabilities))
	{
		probability1 <- probabilities[index, 1]
		probability2 <- probabilities[index, 2]
		probability3 <- probabilities[index, 3]
		probabilityVector <- c(rep(probability1, 4), rep(probability2, 8), rep(probability3, 4))
		capture.output(methodResult <- articulationConditioningSplitting(graph=graph, n = 100000, seed = 1, probabilities = probabilityVector, initialRadius = 3, splittingFactors = rep(1L, 3)))
		exactResult <- exactProbability(graph, probabilityVector)
		expect_equal(as.numeric(methodResult@estimate), as.numeric(exactResult), tolerance = 0.01)
	}
})
