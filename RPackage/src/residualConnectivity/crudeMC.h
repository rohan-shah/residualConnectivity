#ifndef RPACKAGE_CRUDEMC_HEADER_GUARD
#define RPACKAGE_CRUDEMC_HEADER_GUARD
#include "Rcpp.h"
SEXP crudeMC_igraph(SEXP graph, SEXP vertexCoordinates, SEXP probability, SEXP n, SEXP seed);
SEXP crudeMC_graphNEL(SEXP graph, SEXP vertexCoordinates_sexp, SEXP probability, SEXP n, SEXP seed);
SEXP crudeMC_graphAM(SEXP graph, SEXP vertexCoordinates_sexp, SEXP probability, SEXP n, SEXP seed);
#endif
