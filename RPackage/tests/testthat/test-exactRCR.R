context("check exactRCR function")

test_that("Inputs of exactSpectra and exactCounts give the same answer",
{
	probabilities <- seq(0.1, 0.9, length.out=5)
	for(gridSize in 7:9)
	{
		objectName <- paste0("grid", gridSize, "Counts")
		data(list=objectName)
		exactCounts <- get(objectName)
		exactSpectra <- as(exactCounts, "exactSpectra")
		for(probability in probabilities)
		{
			spectraValue <- as.double(exactRCR(exactSpectra, probability))
			countsValue <- as.double(exactRCR(exactCounts, probability))
			expect_equal(countsValue, spectraValue, tolerance=1e-10)
		}
	}
})
