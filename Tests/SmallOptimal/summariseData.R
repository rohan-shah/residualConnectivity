optimal6 <- as.matrix(read.table("./OptimalDensity6.txt", header = TRUE))
pdf("OptimalDensity6.pdf")
plot(optimal6[1,], optimal6[2,], xlab = "Parameter for distribution", ylab = "Parameter for optimal importance sampling", main = "Optimal importance sampling densities for 6x6 grid", xlim = c(0, 1), ylim = c(0, 1))
abline(0, 1)
dev.off()

optimal5 <- as.matrix(read.table("./OptimalDensity5.txt", header = TRUE))
pdf("OptimalDensity5.pdf")
plot(optimal5[1,], optimal5[2,], xlab = "Parameter for distribution", ylab = "Parameter for optimal importance sampling", main = "Optimal importance sampling densities for 5x5 grid", xlim = c(0, 1), ylim = c(0, 1))
abline(0, 1)
dev.off()

optimal5.variance <- as.matrix(read.table("./OptimalDensity5.variance.txt"))
optimal6.variance <- as.matrix(read.table("./OptimalDensity6.variance.txt"))

probabilities = optimal5[1,]

variancePlots <- function(varianceData, optimalParameterData, dimension, varianceFractionScale)
{
#	pdf(paste0("./OptimalDensity", dimension, ".variance.pdf"))
#	image(log(varianceData), main = paste0("Log-variances of using different importance densities for ", dimension, "x", dimension))
#	dev.off()
#
#	pdf(paste0("./OptimalDensity", dimension, ".variance2.pdf"))
#	copied.varianceData <- varianceData
#	copied.varianceData[copied.varianceData > 10] <- 10
#	image(log(copied.varianceData), main = paste0("Log-variances of using different importance densities for ", dimension, "x", dimension))
#	filled.contour(x=probabilities, y = probabilities, varianceData, levels = seq(0, 1, by = 0.05), main = paste0("Variances of using different importance densities for ", dimension, "x", dimension))
#	dev.off()

	pdf(paste0("./OptimalDensity", dimension, ".variance.pdf"))
	filled.contour(x=probabilities, y = probabilities, varianceData, levels = seq(0, 1, by = 0.05), main = paste0("Variances of using different importance densities \nfor ", dimension, "x", dimension), color.palette = heat.colors)
	dev.off()

	pdf(paste0("./varianceReductionFactor", dimension, ".pdf"))
	#Each row of the variance data corresponds to a single true density, and all the importance densities
	optimalVariance = varianceData[cbind(1:nrow(varianceData), match(optimalParameterData[2,], probabilities))]
	noImportanceVariance = diag(varianceData)
	ratio = noImportanceVariance / optimalVariance
	plot(y = ratio, main = "Variance without importance sampling as a fraction \nof optimal importance variance", x = probabilities, xlab = "Parameter", ylab = "Variance multiple", type = "l", ylim = c(0, varianceFractionScale))
	dev.off()
}
varianceFractionScale = 26
variancePlots(optimal5.variance, optimal5, 5, varianceFractionScale)
variancePlots(optimal6.variance, optimal6, 6, varianceFractionScale)