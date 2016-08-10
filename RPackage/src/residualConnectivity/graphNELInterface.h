#ifndef GRAPH_NEL_INTERFACE_HEADER_GUARD
#define GRAPH_NEL_INTERFACE_HEADER_GUARD
#include <Rcpp.h>
#include "context.h"
boost::shared_ptr<residualConnectivity::context::inputGraph> graphNELConvert(SEXP graph_sexp);
#endif
