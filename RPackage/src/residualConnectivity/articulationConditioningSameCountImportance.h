#ifndef RPACKAGE_ARTICULATION_CONDITIONING_SAME_COUNT_IMPORTANCE_HEADER_GUARD
#define RPACKAGE_ARTICULATION_CONDITIONING_SAME_COUNT_IMPORTANCE_HEADER_GUARD
#include "Rcpp.h"
SEXP articulationConditioningSameCountImportance(SEXP importanceProbabilities_sexp, SEXP graph, SEXP probability, SEXP n, SEXP initialRadius, SEXP seed, SEXP finalStepSampleSize, SEXP verbose);
#endif
