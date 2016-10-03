#include "igraphInterface.h"
void igraphConvert(SEXP graph_sexp, residualConnectivity::context::inputGraph& outputGraph)
{
	//Convert graph object
	Rcpp::List graph;
	try
	{
		graph = Rcpp::as<Rcpp::List>(graph_sexp);
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Unable to convert input graph to a list");
	}
	Rcpp::Environment igraphEnv("package:igraph");
	Rcpp::Function length("length"), as_edgelist = igraphEnv["as_edgelist"], is_directed = igraphEnv["is_directed"], V = igraphEnv["V"];
	int nVertices = Rcpp::as<int>(length(V(graph_sexp)));
	bool directed = Rcpp::as<bool>(is_directed(graph_sexp));
	Rcpp::NumericMatrix edgeList = Rcpp::as<Rcpp::NumericMatrix>(as_edgelist(graph_sexp));
	if(directed)
	{
		throw std::runtime_error("Input graph must be undirected");
	}

	//Construct graph
	outputGraph = residualConnectivity::context::inputGraph(nVertices);
	for(int i = 0; i < edgeList.nrow(); i++)
	{
		boost::add_edge(edgeList(i, 0)-1, edgeList(i, 1)-1, outputGraph);
		boost::add_edge(edgeList(i, 1)-1, edgeList(i, 0)-1, outputGraph);
	}
}
