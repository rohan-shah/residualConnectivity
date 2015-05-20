#ifndef GRAPH_NEL_INTERFACE_HEADER_GUARD
#define GRAPH_NEL_INTERFACE_HEADER_GUARD
#include <Rcpp.h>
#include "Context.h"
discreteGermGrain::Context graphNELInterface(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp);
#endif