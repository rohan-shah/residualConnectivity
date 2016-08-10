#ifndef RPACKAGE_GRAPH_INTERFACE_HEADER_GUARD
#define RPACKAGE_GRAPH_INTERFACE_HEADER_GUARD
#include "context.h"
#include "graphType.h"
#include <Rcpp.h>
residualConnectivity::context graphInterface(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp, graphType type);
#endif