context("check spectraToRCR function")

test_that("Check that stochastic enumeration estimate agrees with exact values for 6x6 grid",
{
	probabilities <- seq(0.1, 0.9, length.out=10)
	data(grid6Counts)
	for(optimized in c(TRUE, FALSE))
	{
		stochasticEnumeration <- stochasticEnumeration(grid6Counts@graph, budget = 50000, seed = 1, optimized = optimized)
		for(probability in probabilities)
		{
			estimatedValue <- as.double(spectraToRCR(stochasticEnumeration, probability))
			exactValue <- as.double(exactRCR(grid6Counts, probability))
			smallest <- min(estimatedValue, exactValue)
			expect_equal(estimatedValue, exactValue, tolerance=0.01)
		}
	}
})

test_that("Check that the optimized version has lower relative error",
	{
		probability <- 0.3
		data(grid12Counts)
		optimizedResults <- unoptimizedResults <- c()
		for(index in 1:20)
		{
			optimizedSE <- stochasticEnumeration(grid12Counts@graph, budget = 1000, seed = index, optimized = TRUE, form = "spectra")
			unoptimizedSE <- stochasticEnumeration(grid12Counts@graph, budget = 1000, seed = index, optimized = FALSE, form = "spectra")
			optimizedResults <- c(optimizedResults, spectraToRCR(optimizedSE, probability))
			unoptimizedResults <- c(unoptimizedResults, spectraToRCR(unoptimizedSE, probability))
		}
		optimizedArray <- mpfr2array(optimizedResults, dim=20)
		unoptimizedArray <- mpfr2array(unoptimizedResults, dim=20)
		exact <- exactRCR(grid12Counts, probability)
		expect_less_than(as.double(abs(sum(optimizedArray)/20 - exact)), as.double(abs(sum(unoptimizedArray)/20 - exact)))
	})