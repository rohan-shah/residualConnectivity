context("check crudeMC function")
library(graph)

test_that("Function can be called using graphNEL, graphAM and igraph, and gives same results", 
	{
		data(grid6Counts, package="residualConnectivity")

		igraph <- igraph::graph.lattice(dim = 2, length = 5)
		graphNEL <- igraph::igraph.to.graphNEL(igraph)
		graphAM <- as(graphNEL, "graphAM")

		probabilities <- c(0.1, 0.5, 0.9)
		for(index in 1:length(probabilities))
		{
			probability <- probabilities[index]
			igraphResult <- crudeMC(igraph, probabilities = probability, n = 1000, seed = index)@estimate
			graphAMResult <- crudeMC(graphAM, probabilities = probability, n = 1000, seed = index)@estimate
			graphNELResult <- crudeMC(graphNEL, probabilities = probability, n = 1000, seed = index)@estimate

			expect_identical(igraphResult, graphAMResult)
			expect_identical(graphAMResult, graphNELResult)
		}
	})

test_that("Test that crudeMC gives numerically accurate results", 
	{
		data(grid4Counts, package="residualConnectivity")
		igraph <- igraph::graph.lattice(dim = 2, length = 4)
		probabilities <- seq(from = 0.1, to = 0.9, length.out = 5)
		for(index in 1:length(probabilities))
		{
			probability <- probabilities[index]
			igraphResult <- crudeMC(igraph, probabilities = probability, n = 500000, seed = index)@estimate

			exactResult <- as.double(exactRCR(grid4Counts, probability))

			expect_equal(igraphResult, exactResult, tolerance = 0.003)
		}
	})
