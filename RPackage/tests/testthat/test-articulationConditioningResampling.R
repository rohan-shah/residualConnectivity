context("check articulationConditioningResampling function")
library(graph)

test_that("Function can be called using graphNEL, graphAM and igraph, and gives same results", 
	{
		gridSizes <- 5:6
		probabilities <- c(0.3, 0.6)
		for(gridSize in gridSizes)
		{
			igraph <- igraph::graph.lattice(dim = 2, length=gridSize)
			graphNEL <- igraph::igraph.to.graphNEL(igraph)
			graphAM <- as(graphNEL, "graphAM")
			for(probability in probabilities)
			{

				capture.output(result_igraph <- articulationConditioningResampling(graph=igraph, n = 1000, seed = gridSize, initialRadius = 3, probability = probability))
				capture.output(result_graphNEL <- articulationConditioningResampling(graph=graphNEL, n = 1000, seed = gridSize, initialRadius = 3, probability = probability))
				capture.output(result_graphAM <- articulationConditioningResampling(graph=graphAM, n = 1000, seed = gridSize, initialRadius = 3, probability = probability))
				expect_identical(result_igraph@estimate, result_graphNEL@estimate)
				expect_identical(result_graphNEL@estimate, result_graphAM@estimate)
			}
		}
	})
test_that("Values agree with exact values for small grids and uniform probabilities",
{
	probabilities <- seq(0.1, 0.9, length.out = 9)
	for(gridSize in 3:6)
	{
		graph <- igraph::graph.lattice(dim = 2, length=gridSize)
		data(list = paste0("grid", gridSize, "Counts"), package="residualConnectivity")
		subgraphCounts <- get(paste0("grid", gridSize, "Counts"))
		for(probability in probabilities)
		{
			capture.output(methodResult <- articulationConditioningResampling(graph=graph, n = 20000, seed = gridSize, probability = probability, initialRadius = 3))
			exactResult <- exactRCR(subgraphCounts, probability)
			expect_equal(as.numeric(methodResult@estimate), as.numeric(exactResult), tolerance = 3e-2)
		}
	}
})
