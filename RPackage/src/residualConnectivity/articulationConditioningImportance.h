#ifndef RPACKAGE_ARTICULATION_CONDITIONING_IMPORTANCE_HEADER_GUARD
#define RPACKAGE_ARTICULATION_CONDITIONING_IMPORTANCE_HEADER_GUARD
#include "Rcpp.h"
SEXP articulationConditioningImportance(SEXP importanceProbabilities, SEXP graph, SEXP probability, SEXP n, SEXP initialRadius, SEXP seed, SEXP verbose, SEXP finalStepSampleSize_sexp);
#endif
