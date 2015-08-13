context("check PMC function")

test_that("Check that PMC estimate agrees with exact values for 5x5 grid",
{
	probabilities <- seq(0.1, 0.9, length.out=10)
	data(grid5Counts)
	pmcResult <- PMC(grid5Counts@graph, n = 10000, seed = 1)
	for(probability in probabilities)
	{
		estimatedValue <- as.double(spectraToRCR(pmcResult, probability))
		exactValue <- as.double(exactRCR(grid5Counts, probability))
		expect_equal(estimatedValue, exactValue, tolerance=0.03)
	}
})