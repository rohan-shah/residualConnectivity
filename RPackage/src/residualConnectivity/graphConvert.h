#ifndef RPACKAGE_GRAPH_CONVERT_HEADER_GUARD
#define RPACKAGE_GRAPH_CONVERT_HEADER_GUARD
#include "Context.h"
#include "graphType.h"
boost::shared_ptr<residualConnectivity::Context::inputGraph> graphConvert(SEXP graph_sexp, graphType type);
#endif