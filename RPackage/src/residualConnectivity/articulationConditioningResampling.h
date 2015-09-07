#ifndef RPACKAGE_ARTICULATION_CONDITIONING_RESAMPLING_HEADER_GUARD
#define RPACKAGE_ARTICULATION_CONDITIONING_RESAMPLING_HEADER_GUARD
#include "Rcpp.h"
SEXP articulationConditioningResampling_igraph(SEXP graph, SEXP vertexCoordinates_sexp, SEXP probability, SEXP n, SEXP initialRadius, SEXP seed);
SEXP articulationConditioningResampling_graphNEL(SEXP graph, SEXP vertexCoordinates_sexp, SEXP probability, SEXP n, SEXP initialRadius, SEXP seed);
SEXP articulationConditioningResampling_graphAM(SEXP graph, SEXP vertexCoordinates_sexp, SEXP probability, SEXP n, SEXP initialRadius, SEXP seed);
#endif
