#include "exhaustiveSearchUnequalProbabilities.h"
#include "exactMethods/exhaustiveSearchUnequalProbabilities.h"
#include "graphConvert.h"
#include "graphType.h"
#include <sstream>
SEXP exhaustiveSearchUnequalProbabilities(SEXP graph, SEXP probabilities_sexp, graphType type)
{
BEGIN_RCPP
	boost::shared_ptr<residualConnectivity::context::inputGraph> boostGraph = graphConvert(graph, type);
	Rcpp::NumericVector probabilities_Rcpp = Rcpp::as<Rcpp::NumericVector>(probabilities_sexp);
	std::vector<mpfr_class> probabilities;
	std::transform(probabilities_Rcpp.begin(), probabilities_Rcpp.end(), std::back_inserter(probabilities), [](double x){return mpfr_class(x);});

	std::string message;
	mpfr_class result = 0;
	residualConnectivity::exhaustiveSearchUnequalProbabilities(*boostGraph.get(), probabilities, result, message);
	return Rcpp::wrap(result.str());
END_RCPP
}
SEXP exhaustiveSearchUnequalProbabilities_igraph(SEXP graph, SEXP probabilities_sexp)
{
	return exhaustiveSearchUnequalProbabilities(graph, probabilities_sexp, IGRAPH);
}
SEXP exhaustiveSearchUnequalProbabilities_graphNEL(SEXP graph, SEXP probabilities_sexp)
{
	return exhaustiveSearchUnequalProbabilities(graph, probabilities_sexp, GRAPHNEL);
}
SEXP exhaustiveSearchUnequalProbabilities_graphAM(SEXP graph, SEXP probabilities_sexp)
{
	return exhaustiveSearchUnequalProbabilities(graph, probabilities_sexp, GRAPHAM);
}
