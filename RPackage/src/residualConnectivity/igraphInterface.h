#ifndef IGRAPH_INTERFACE_HEADER_GUARD
#define IGRAPH_INTERFACE_HEADER_GUARD
#include <Rcpp.h>
#include "Context.h"
enum graphType
{
	IGRAPH, GRAPHNEL, GRAPHAM
};
discreteGermGrain::Context igraphInterface(SEXP graph_sexp, SEXP vertexCoordinates_sexp, SEXP probability_sexp);
boost::shared_ptr<discreteGermGrain::Context::inputGraph> igraphConvert(SEXP graph_sexp);
#endif
