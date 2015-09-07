context("validate gridCounts data")

test_that("There is no grid13Counts object", 
	{
		expect_that({data(grid13Counts)}, gives_warning())
	})
test_that("Data objects are present",
{
	for(i in 3:12)
	{
		#These objects exist
		expect_identical({data(list = paste0("grid", i, "Counts"), package="residualConnectivity")}, paste0("grid", i, "Counts"))
		expect_identical({data(list = paste0("grid", i, "Counts"), package="residualConnectivity"); exists(paste0("grid", i, "Counts"))}, TRUE)
	}
})
