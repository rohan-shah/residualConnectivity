#include "createHexagonalLattice.h"
#include "constructGraphs.h"
SEXP createHexagonalLattice(SEXP x_sexp, SEXP y_sexp)
{
BEGIN_RCPP
	int x = Rcpp::as<int>(x_sexp), y = Rcpp::as<int>(y_sexp);

	boost::shared_ptr<std::vector<residualConnectivity::context::vertexPosition> > vertexPositions;
	boost::shared_ptr<residualConnectivity::context::inputGraph> graph;
	residualConnectivity::constructGraphs::hexagonalTiling(x, y, graph, vertexPositions);

	std::size_t nEdges = boost::num_edges(*graph), nVertices = boost::num_vertices(*graph);
	Rcpp::NumericMatrix edges(2, (int)nEdges), positions((int)nVertices, 2);

	residualConnectivity::context::inputGraph::edge_iterator current, end;
	boost::tie(current, end) = boost::edges(*graph);
	int index = 0;
	while(current != end)
	{
		edges(0, index) = (float)current->m_target+1;
		edges(1, index) = (float)current->m_source+1;
		index++;
		current++;
	}

	for (int i = 0; i < (int)nVertices; i++)
	{
		positions(i, 0) = (*vertexPositions)[i].first;
		positions(i, 1) = (*vertexPositions)[i].second;
	}
	Rcpp::List returnValue = Rcpp::List::create(Rcpp::Named("edges") = edges, Rcpp::Named("positions") = positions);
	return returnValue;
END_RCPP
}