#ifndef RPACKAGE_ARTICULATION_CONDITIONING_HEADER_GUARD
#define RPACKAGE_ARTICULATION_CONDITIONING_HEADER_GUARD
#include "Rcpp.h"
SEXP articulationConditioning(SEXP graph, SEXP probability, SEXP n, SEXP initialRadius, SEXP seed, SEXP verbose, SEXP nLastStep);
#endif
