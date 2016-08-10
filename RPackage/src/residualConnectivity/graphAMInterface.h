#ifndef GRAPH_AM_INTERFACE_HEADER_GUARD
#define GRAPH_AM_INTERFACE_HEADER_GUARD
#include <Rcpp.h>
#include "context.h"
boost::shared_ptr<residualConnectivity::context::inputGraph> graphAMConvert(SEXP graph_sexp);
#endif
