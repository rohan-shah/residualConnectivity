#ifndef RPACKAGE_RECURSIVE_VARIANCE_REDUCTION_HEADER_GUARD
#define RPACKAGE_RECURSIVE_VARIANCE_REDUCTION_HEADER_GUARD
#include "Rcpp.h"
SEXP recursiveVarianceReduction_igraph(SEXP graph, SEXP vertexCoordinates, SEXP probability, SEXP n, SEXP seed);
SEXP recursiveVarianceReduction_graphNEL(SEXP graph, SEXP vertexCoordinates_sexp, SEXP probability, SEXP n, SEXP seed);
SEXP recursiveVarianceReduction_graphAM(SEXP graph, SEXP vertexCoordinates_sexp, SEXP probability, SEXP n, SEXP seed);
#endif
