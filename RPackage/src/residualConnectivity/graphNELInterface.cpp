#include "graphNELInterface.h"
discreteGermGrain::Context graphNELInterface(SEXP graph_sexp, SEXP probability_sexp)
{
	//Convert probability
	double probability;
	try
	{
		probability = Rcpp::as<double>(probability_sexp);
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Unable to convert input probability to a number");
	}

	Rcpp::S4 graph_s4;
	try
	{
		graph_s4 = Rcpp::as<Rcpp::S4>(graph_sexp);
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Input graph must be an S4 object");
	}
	if(Rcpp::as<std::string>(graph_s4.attr("class")) != "graphNEL")
	{
		throw std::runtime_error("Input graph must have class graphNEL");
	}
	
	Rcpp::RObject nodes_obj;
	try
	{
		nodes_obj = Rcpp::as<Rcpp::RObject>(graph_s4.slot("nodes"));
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Error extracting slot nodes");
	}

	Rcpp::RObject edges_obj;
	try
	{
		edges_obj = Rcpp::as<Rcpp::RObject>(graph_s4.slot("edgeL"));
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Error extracting slot edgeL");
	}

	Rcpp::CharacterVector nodeNames;
	try
	{
		nodeNames = Rcpp::as<Rcpp::CharacterVector>(nodes_obj);
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Slot nodes of input graph must be a character vector");
	}
	{
		std::vector<std::string> uniqueNodeNames = Rcpp::as<std::vector<std::string> >(nodeNames);
		std::sort(uniqueNodeNames.begin(), uniqueNodeNames.end());
		uniqueNodeNames.erase(std::unique(uniqueNodeNames.begin(), uniqueNodeNames.end()), uniqueNodeNames.end());
		if(uniqueNodeNames.size() != nodeNames.size())
		{
			throw std::runtime_error("Node names of input graph were not unique");
		}
	}
	int nVertices = nodeNames.size();

	Rcpp::List edges_list;
	try
	{
		edges_list = Rcpp::as<Rcpp::List>(edges_obj);
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Slot edgeL of input graph must be a list");
	}
	Rcpp::CharacterVector edges_list_names = Rcpp::as<Rcpp::CharacterVector>(edges_list.attr("names"));

	boost::shared_ptr<discreteGermGrain::Context::inputGraph> boostGraph(new discreteGermGrain::Context::inputGraph(nVertices));
	discreteGermGrain::Context::inputGraph& boostGraphRef = *boostGraph;
	for(int i = 0; i < edges_list.size(); i++)
	{
		int nodeIndex = std::distance(nodeNames.begin(), std::find(nodeNames.begin(), nodeNames.end(), edges_list_names(i)));
		Rcpp::List subList;
		Rcpp::CharacterVector subListNames;
		try
		{
			subList = Rcpp::as<Rcpp::List>(edges_list(i));
			subListNames = Rcpp::as<Rcpp::CharacterVector>(subList.attr("names"));
		}
		catch(Rcpp::not_compatible&)
		{
			throw std::runtime_error("Slot edgeL of input graph had an invalid format");
		}
		if(std::find(subListNames.begin(), subListNames.end(), "edges") == subListNames.end())
		{
			throw std::runtime_error("Slot edgeL of input graph had an invalid format");
		}
		Rcpp::NumericVector targetIndicesThisNode;
		try
		{
			targetIndicesThisNode = Rcpp::as<Rcpp::NumericVector>(subList("edges"));
		}
		catch(Rcpp::not_compatible&)
		{
			throw std::runtime_error("Slot edgeL of input graph had an invalid format");
		}
		for(int j = 0; j < targetIndicesThisNode.size(); j++)
		{
			boost::add_edge((std::size_t)nodeIndex, (std::size_t)((int)targetIndicesThisNode(j)-1), boostGraphRef);
		}
	}

	boost::shared_ptr<std::vector<int> > defaultOrdering(new std::vector<int>());
	std::vector<int>& defaultOrderingRef = *defaultOrdering;
	for(int i = 0; i < nVertices; i++)
	{
		defaultOrderingRef.push_back(i);
	}

	boost::shared_ptr<std::vector<discreteGermGrain::Context::vertexPosition> > vertexCoordinates(new std::vector<discreteGermGrain::Context::vertexPosition>());
	std::vector<discreteGermGrain::Context::vertexPosition>& vertexCoordinatesRef = *vertexCoordinates;
	vertexCoordinatesRef.reserve(nVertices);
	float increment = (float)(2.0*M_PI/nVertices);
	for(int i = 0; i < nVertices; i++)
	{
		float angle = i * increment;
		vertexCoordinatesRef.push_back(discreteGermGrain::Context::vertexPosition(cosf(angle), sinf(angle)));
	}

	discreteGermGrain::Context context(boostGraph, defaultOrdering, vertexCoordinates, probability);
	return context;
}