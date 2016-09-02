context("check exactProbability function")

test_that("Check that exactProbability agrees with the code based on spectra",
{
	probabilities <- seq(0.1, 0.9, length.out=3)
	for(gridSize in 3:4)
	{
		objectName <- paste0("grid", gridSize, "Counts")
		data(list=objectName)
		exactCounts <- get(objectName)
		for(probability in probabilities)
		{
			computedValue <- as.double(exactProbability(exactCounts@graph, probability))
			spectraValue <- as.double(exactRCR(exactCounts, probability))
			expect_equal(computedValue, spectraValue, tolerance=1e-10)
		}
	}
})
