#ifndef RPACKAGE_ARTICULATION_CONDITIONING_SPLITTING_HEADER_GUARD
#define RPACKAGE_ARTICULATION_CONDITIONING_SPLITTING_HEADER_GUARD
#include "Rcpp.h"
SEXP articulationConditioningSplitting_igraph(SEXP graph, SEXP vertexCoordinates_sexp, SEXP probability, SEXP n, SEXP initialRadius, SEXP seed, SEXP splittingFactors);
SEXP articulationConditioningSplitting_graphNEL(SEXP graph, SEXP vertexCoordinates_sexp, SEXP probability, SEXP n, SEXP initialRadius, SEXP seed, SEXP splittingFactors);
SEXP articulationConditioningSplitting_graphAM(SEXP graph, SEXP vertexCoordinates_sexp, SEXP probability, SEXP n, SEXP initialRadius, SEXP seed, SEXP splittingFactors);
#endif
