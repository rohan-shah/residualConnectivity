setClassUnion("mpfrOrNumeric", c("mpfr", "numeric"))
setClass("monteCarloResult", slots = list(estimate = "mpfrOrNumeric", call = "call", start = "POSIXct", end = "POSIXct"))
setMethod(f = "show", signature = "monteCarloResult", definition = function(object)
{
	cat("Monte Carlo estimate of ", as.numeric(object@estimate), " generated in ", as.numeric(object@end - object@start, unit="secs"), " seconds, by\n", sep="")
	print(object@call)
})
