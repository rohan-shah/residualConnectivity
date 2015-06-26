#ifndef CRUDEMC_INTERFACES_HEADER_GUARD
#define CRUDEMC_INTERFACES_HEADER_GUARD
#include "Rcpp.h"
RcppExport SEXP crudeMC_igraph(SEXP graph, SEXP vertexCoordinates, SEXP probability, SEXP n, SEXP seed);
RcppExport SEXP crudeMC_graphNEL(SEXP graph, SEXP vertexCoordinates_sexp, SEXP probability, SEXP n, SEXP seed);
RcppExport SEXP crudeMC_graphAM(SEXP graph, SEXP vertexCoordinates_sexp, SEXP probability, SEXP n, SEXP seed);
#endif
