#ifndef EXHAUSTIVE_SEARCH_INTERFACES_HEADER_GUARD
#define EXHAUSTIVE_SEARCH_INTERFACES_HEADER_GUARD
#include "Rcpp.h"
SEXP exhaustiveSearch_igraph(SEXP graph);
SEXP exhaustiveSearch_graphNEL(SEXP graph);
SEXP exhaustiveSearch_graphAM(SEXP graph);
#endif
