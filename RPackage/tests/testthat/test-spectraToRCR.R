context("check spectraToRCR function")

test_that("Check that stochastic enumeration estimate agrees with exact values for 6x6 grid",
{
	probabilities <- seq(0.1, 0.9, length.out=10)
	data(grid6Counts)
	stochasticEnumeration <- stochasticEnumeration(grid6Counts@graph, budget = 50000, seed = 1)
	for(probability in probabilities)
	{
		estimatedValue <- as.double(spectraToRCR(stochasticEnumeration, probability))
		exactValue <- as.double(exactRCR(grid6Counts, probability))
		smallest <- min(estimatedValue, exactValue)
		expect_equal(estimatedValue, exactValue, tolerance=0.011)
	}
})