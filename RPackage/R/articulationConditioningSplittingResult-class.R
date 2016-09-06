#' @include monteCarloResult-class.R
setClass("articulationConditioningSplittingResult", contains = "monteCarloResult", slots = list(levelProbabilities = "numeric"))
