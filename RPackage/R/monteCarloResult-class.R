setClassUnion("mpfrOrNumeric", c("mpfr", "numeric"))
setClass("monteCarloResult", slots = list(estimate = "mpfrOrNumeric", call = "call", start = "POSIXct", end = "POSIXct"))
