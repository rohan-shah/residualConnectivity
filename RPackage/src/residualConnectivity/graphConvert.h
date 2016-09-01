#ifndef RPACKAGE_GRAPH_CONVERT_HEADER_GUARD
#define RPACKAGE_GRAPH_CONVERT_HEADER_GUARD
#include "context.h"
void graphConvert(SEXP graph_sexp, residualConnectivity::context::inputGraph& graph, std::vector<residualConnectivity::context::vertexPosition>& vertexCoordinates);
#endif
