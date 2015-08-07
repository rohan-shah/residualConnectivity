#ifndef IGRAPH_INTERFACE_HEADER_GUARD
#define IGRAPH_INTERFACE_HEADER_GUARD
#include <Rcpp.h>
#include "Context.h"
boost::shared_ptr<discreteGermGrain::Context::inputGraph> igraphConvert(SEXP graph_sexp);
#endif
