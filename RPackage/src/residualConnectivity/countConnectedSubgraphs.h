#ifndef COUNT_CONNECTED_SUBGRAPHS_INTERFACES_HEADER_GUARD
#define COUNT_CONNECTED_SUBGRAPHS_INTERFACES_HEADER_GUARD
#include "Rcpp.h"
SEXP countConnectedSubgraphs_igraph(SEXP graph);
SEXP countConnectedSubgraphs_graphNEL(SEXP graph);
SEXP countConnectedSubgraphs_graphAM(SEXP graph);
#endif
