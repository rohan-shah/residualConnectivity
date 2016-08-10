#ifndef IGRAPH_INTERFACE_HEADER_GUARD
#define IGRAPH_INTERFACE_HEADER_GUARD
#include <Rcpp.h>
#include "context.h"
boost::shared_ptr<residualConnectivity::context::inputGraph> igraphConvert(SEXP graph_sexp);
#endif
