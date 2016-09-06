context("check countConnectedSubgraphsBySize function")
library(graph)

test_that("Function can be called using graphNEL, graphAM and igraph, and gives same results", 
	{
		gridSizes <- 2:4
		for(gridSize in gridSizes)
		{
			igraph <- igraph::graph.lattice(dim = 2, length=gridSize)
			graphNEL <- igraph::igraph.to.graphNEL(igraph)
			graphAM <- as(graphNEL, "graphAM")

			counts_igraph <- countConnectedSubgraphsBySize(igraph)
			counts_graphNEL <- countConnectedSubgraphsBySize(graphNEL)
			counts_graphAM <- countConnectedSubgraphsBySize(graphAM)

			expect_identical(counts_igraph@counts, counts_graphNEL@counts)
			expect_identical(counts_graphNEL@counts, counts_graphAM@counts)
		}
	})

test_that("Values agree with transfer matrix data for small grids",
{
	for(i in 3:4)
	{
		objectName <- data(list=paste0("grid", i, "Counts"))
		exact <- get(objectName)

		graph <- igraph::graph.lattice(dim = 2, length=i)
		subgraphCounts <- countConnectedSubgraphsBySize(graph)
		
		#These objects are now identical
		expect_identical(exact@counts, subgraphCounts@counts)
	}
})
