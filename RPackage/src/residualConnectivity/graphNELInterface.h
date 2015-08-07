#ifndef GRAPH_NEL_INTERFACE_HEADER_GUARD
#define GRAPH_NEL_INTERFACE_HEADER_GUARD
#include <Rcpp.h>
#include "Context.h"
boost::shared_ptr<discreteGermGrain::Context::inputGraph> graphNELConvert(SEXP graph_sexp);
#endif
