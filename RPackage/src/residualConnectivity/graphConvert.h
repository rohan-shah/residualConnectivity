#ifndef RPACKAGE_GRAPH_CONVERT_HEADER_GUARD
#define RPACKAGE_GRAPH_CONVERT_HEADER_GUARD
#include "context.h"
#include "graphType.h"
boost::shared_ptr<residualConnectivity::context::inputGraph> graphConvert(SEXP graph_sexp, graphType type);
#endif