#ifndef CRUDEMC_INTERFACES_HEADER_GUARD
#define CRUDEMC_INTERFACES_HEADER_GUARD
#include "Rcpp.h"
SEXP crudeMC_igraph(SEXP graph, SEXP vertexCoordinates, SEXP probability, SEXP n, SEXP seed);
#endif