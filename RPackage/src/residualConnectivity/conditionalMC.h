#ifndef RPACKAGE_CONDITIONALMC_HEADER_GUARD
#define RPACKAGE_CONDITIONALMC_HEADER_GUARD
#include "Rcpp.h"
SEXP conditionalMC_igraph(SEXP graph, SEXP vertexCoordinates_sexp, SEXP probability, SEXP n, SEXP seed);
SEXP conditionalMC_graphNEL(SEXP graph, SEXP vertexCoordinates_sexp, SEXP probability, SEXP n, SEXP seed);
SEXP conditionalMC_graphAM(SEXP graph, SEXP vertexCoordinates_sexp, SEXP probability, SEXP n, SEXP seed);
#endif
