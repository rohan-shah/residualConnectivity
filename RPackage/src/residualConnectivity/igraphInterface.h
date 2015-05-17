#ifndef IGRAPH_INTERFACE_HEADER_GUARD
#define IGRAPH_INTERFACE_HEADER_GUARD
#include <Rcpp.h>
#include "Context.h"
discreteGermGrain::Context igraphInterface(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp);
#endif