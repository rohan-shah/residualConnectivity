context("check conditionalMC function")

test_that("Function can be called using graphNEL, graphAM and igraph, and gives same results", 
	{
		data(grid6Counts, package="residualConnectivity")
		library(igraph)

		igraph <- graph.lattice(dim = 2, length = 6)
		graphNEL <- igraph.to.graphNEL(igraph)
		graphAM <- as(graphNEL, "graphAM")

		probabilities <- c(0.1, 0.5, 0.9)
		for(index in 1:length(probabilities))
		{
			probability <- probabilities[index]
			igraphResult <- conditionalMC(igraph, probability = probability, n = 1000, seed = index)
			graphAMResult <- conditionalMC(graphAM, probability = probability, n = 1000, seed = index)
			graphNELResult <- conditionalMC(graphNEL, probability = probability, n = 1000, seed = index)

			expect_identical(igraphResult, graphAMResult)
			expect_identical(graphAMResult, graphNELResult)
		}
	})

test_that("Test that conditionalMC gives numerically accurate results", 
	{
		library(igraph)
		data(grid4Counts, package="residualConnectivity")
		igraph <- graph.lattice(dim = 2, length = 4)
		probabilities <- seq(from = 0.1, to = 0.9, length.out = 5)
		for(index in 1:length(probabilities))
		{
			probability <- probabilities[index]
			igraphResult <- conditionalMC(igraph, probability = probability, n = 500000, seed = index)

			exactResult <- as.double(exactRCR(grid4Counts, probability))

			expect_equal(igraphResult, exactResult, tolerance = 0.002)
		}
	})