context("check stochasticEnumeration function")

test_that("Function can be called using graphNEL, graphAM and igraph, and gives same results", 
	{
		library(graph)
		gridSizes <- 2:4
		forms <- c("spectra", "counts")
		for(form in forms)
		{
			for(gridSize in gridSizes)
			{
				igraph <- graph.lattice(dim = 2, length=gridSize)
				graphNEL <- igraph.to.graphNEL(igraph)
				graphAM <- as(graphNEL, "graphAM")

				estimate_igraph <- stochasticEnumeration(igraph, budget = 1000, form = form, seed = gridSize)
				estimate_graphNEL <- stochasticEnumeration(graphNEL, budget = 1000, form = form, seed = gridSize)
				estimate_graphAM <- stochasticEnumeration(graphAM, budget = 1000, form = form, seed = gridSize)

				expect_identical(estimate_igraph@data, estimate_graphNEL@data)
				expect_identical(estimate_graphNEL@data, estimate_graphAM@data)
			}
		}
	})
test_that("Values agree with exact counts for small grids",
{
	library(igraph)
	library(graph)
	for(i in 2:4)
	{
		graph <- graph.lattice(dim = 2, length=i)
		subgraphCounts <- countConnectedSubgraphs(graph)
		stochasticEnumerationCounts <- stochasticEnumeration(graph, budget = 10000, form = "counts", seed = i)
		expect_identical(as.integer(subgraphCounts@counts), as.integer(stochasticEnumerationCounts@data))
	}
})

test_that("Values agree with exact spectra for small grids",
{
	library(igraph)
	library(graph)
	for(i in 2:4)
	{
		graph <- graph.lattice(dim = 2, length=i)
		subgraphCounts <- countConnectedSubgraphs(graph)
		exactSpectra <- as(subgraphCounts, "exactSpectra")
		stochasticEnumerationCounts <- stochasticEnumeration(graph, budget = 10000, form = "spectra", seed = i)
		expect_equal(as.double(exactSpectra@spectra), as.double(stochasticEnumerationCounts@data), tolerance = 1e-10)
	}
})
