context("check spectraToRCR function")

test_that("Check that stochastic enumeration estimate agrees with exact values for 5x5 grid",
{
	probabilities <- seq(0.1, 0.9, length.out=10)
	data(grid5Counts)
	for(optimized in c(TRUE, FALSE))
	{
		for(nPermutations in c(1, 3))
		{
			stochasticEnumeration <- stochasticEnumeration(grid5Counts@graph, budget = 10000, seed = 1, optimized = optimized, nPermutations = nPermutations)
			for(probability in probabilities)
			{
				estimatedValue <- as.double(spectraToRCR(stochasticEnumeration, probability))
				exactValue <- as.double(exactRCR(grid5Counts, probability))
				expect_equal(estimatedValue, exactValue, tolerance=0.03)
			}
		}
	}
})

test_that("Check that the optimized version has lower bias",
	{
		probability <- 0.3
		data(grid6Counts)
		optimizedResults <- unoptimizedResults <- c()
		for(index in 1:5)
		{
			optimizedSE <- stochasticEnumeration(grid6Counts@graph, budget = 1000, seed = index, optimized = TRUE, form = "spectra")
			unoptimizedSE <- stochasticEnumeration(grid6Counts@graph, budget = 1000, seed = index, optimized = FALSE, form = "spectra")
			optimizedResults <- c(optimizedResults, spectraToRCR(optimizedSE, probability))
			unoptimizedResults <- c(unoptimizedResults, spectraToRCR(unoptimizedSE, probability))
		}
		optimizedArray <- mpfr2array(optimizedResults, dim=5)
		unoptimizedArray <- mpfr2array(unoptimizedResults, dim=5)
		exact <- exactRCR(grid6Counts, probability)
		expect_less_than(as.double(abs(sum(optimizedArray)/5 - exact)), as.double(abs(sum(unoptimizedArray)/5 - exact)))
	})
