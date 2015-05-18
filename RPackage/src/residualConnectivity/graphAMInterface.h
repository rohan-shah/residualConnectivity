#ifndef GRAPH_AM_INTERFACE_HEADER_GUARD
#define GRAPH_AM_INTERFACE_HEADER_GUARD
#include <Rcpp.h>
#include "Context.h"
discreteGermGrain::Context graphAMInterface(SEXP graph_sexp, SEXP probability_sexp);
#endif