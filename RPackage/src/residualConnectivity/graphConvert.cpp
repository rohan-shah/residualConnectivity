#include "graphInterface.h"
#include "igraphInterface.h"
#include "graphAMInterface.h"
#include "graphNELInterface.h"
void graphConvert(SEXP graph_sexp, residualConnectivity::context::inputGraph& boostGraph, std::vector<residualConnectivity::context::vertexPosition>& vertexCoordinates)
{
	Rcpp::RObject graph = Rcpp::as<Rcpp::RObject>(graph_sexp);
	std::string className = Rcpp::as<std::string>(graph.attr("class"));
	Rcpp::NumericMatrix vertexCoordinates_matrix;
	if(className == "igraph")
	{
		Rcpp::Environment igraphEnv("package:igraph");
		Rcpp::Function isDirected = igraphEnv["is_directed"];
		if(Rcpp::as<bool>(isDirected(graph)))
		{
			throw std::runtime_error("Input `graph' must be undirected");
		}
		Rcpp::Function layoutAuto = igraphEnv["layout.auto"];
		vertexCoordinates_matrix = layoutAuto(graph);
		igraphConvert(graph_sexp, boostGraph);
	}
	else if(className == "graphNEL" || className == "graphAM")
	{
		Rcpp::S4 graphS4 = Rcpp::as<Rcpp::S4>(graph);
		Rcpp::S4 renderInfo = Rcpp::as<Rcpp::S4>(graphS4.slot("renderInfo"));
		Rcpp::List nodes = Rcpp::as<Rcpp::List>(renderInfo.slot("nodes"));
		Rcpp::Function length("length"), cbind("cbind");
		if(Rcpp::as<int>(length(nodes)) > 0)
		{
			vertexCoordinates_matrix = Rcpp::as<Rcpp::NumericMatrix>(cbind(nodes["nodeX"], nodes["nodeY"]));
		}
		if(className == "graphNEL") graphNELConvert(graph_sexp, boostGraph);
		else graphAMConvert(graph_sexp, boostGraph);
	}
	else
	{
		throw std::runtime_error("Input graph must have class \"igraph\", \"graphAM\" or \"graphNEL\"");
	}
	std::size_t nVertexCoordinates = vertexCoordinates_matrix.nrow();
	vertexCoordinates.clear();
	vertexCoordinates.reserve(nVertexCoordinates);
	for(std::size_t i = 0; i < nVertexCoordinates; i++)
	{
		vertexCoordinates.push_back(residualConnectivity::context::vertexPosition((residualConnectivity::context::vertexPosition::first_type)vertexCoordinates_matrix((int)i, 0), (residualConnectivity::context::vertexPosition::second_type)vertexCoordinates_matrix((int)i, 1)));
	}
}
