#include "graphInterface.h"
#include "igraphInterface.h"
#include "graphAMInterface.h"
#include "graphNELInterface.h"
boost::shared_ptr<residualConnectivity::context::inputGraph> graphConvert(SEXP graph_sexp, graphType type)
{
	if(type == IGRAPH) return igraphConvert(graph_sexp);
	else if(type == GRAPHNEL) return graphNELConvert(graph_sexp);
	else if(type == GRAPHAM) return graphAMConvert(graph_sexp);
	else
	{
		throw std::runtime_error("Internal error");
	}
}