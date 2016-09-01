#ifndef RPACKAGE_GRAPH_CONVERT_HEADER_GUARD
#define RPACKAGE_GRAPH_CONVERT_HEADER_GUARD
#include "context.h"
#include "graphType.h"
void graphConvert(SEXP graph_sexp, residualConnectivity::context::inputGraph& graph, std::vector<residualConnectivity::context::vertexPosition>& vertexCoordinates);
void graphConvert(SEXP graph_sexp, graphType type, residualConnectivity::context::inputGraph& outputGraph);
#endif
