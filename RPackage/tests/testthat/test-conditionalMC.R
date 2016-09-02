context("check conditionalMC function")
library(graph)

test_that("Function can be called using graphNEL, graphAM and igraph, and gives same results", 
	{
		igraph <- igraph::graph.lattice(dim = 2, length = 5)
		graphNEL <- igraph::igraph.to.graphNEL(igraph)
		graphAM <- as(graphNEL, "graphAM")

		probabilities <- c(0.1, 0.5, 0.9)
		for(index in 1:length(probabilities))
		{
			probability <- probabilities[index]
			igraphResult <- conditionalMC(igraph, probabilities = probability, n = 1000, seed = index)@estimate
			graphAMResult <- conditionalMC(graphAM, probabilities = probability, n = 1000, seed = index)@estimate
			graphNELResult <- conditionalMC(graphNEL, probabilities = probability, n = 1000, seed = index)@estimate

			expect_identical(igraphResult, graphAMResult)
			expect_identical(graphAMResult, graphNELResult)
		}
	})

test_that("Test that conditionalMC gives numerically accurate results", 
	{
		data(grid4Counts, package="residualConnectivity")
		igraph <- igraph::graph.lattice(dim = 2, length = 4)
		probabilities <- seq(from = 0.1, to = 0.9, length.out = 5)
		for(index in 1:length(probabilities))
		{
			probability <- probabilities[index]
			igraphResult <- conditionalMC(igraph, probabilities = probability, n = 500000, seed = index)@estimate

			exactResult <- as.double(exactRCR(grid4Counts, probability))

			expect_equal(igraphResult, exactResult, tolerance = 0.01)
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
		methodResult <- conditionalMC(graph=graph, n = 200000, seed = 1, probabilities = probabilityVector)
		exactResult <- exactProbability(graph, probabilityVector)
		expect_equal(as.numeric(methodResult@estimate), as.numeric(exactResult), tolerance = 0.01)
	}
})
