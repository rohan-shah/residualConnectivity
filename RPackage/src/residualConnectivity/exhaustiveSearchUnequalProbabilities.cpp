#include "exhaustiveSearchUnequalProbabilities.h"
#include "exactMethods/exhaustiveSearchUnequalProbabilities.h"
#include "graphConvert.h"
#include "graphType.h"
#include <sstream>
SEXP exhaustiveSearchUnequalProbabilities(SEXP graph, SEXP probabilities_sexp)
{
BEGIN_RCPP
	boost::shared_ptr<residualConnectivity::context::inputGraph> boostGraph(new residualConnectivity::context::inputGraph());
	std::vector<residualConnectivity::context::vertexPosition> vertexCoordinates;
	graphConvert(graph, *boostGraph.get(), vertexCoordinates);
	Rcpp::NumericVector probabilities_Rcpp = Rcpp::as<Rcpp::NumericVector>(probabilities_sexp);
	std::vector<mpfr_class> probabilities;
	std::transform(probabilities_Rcpp.begin(), probabilities_Rcpp.end(), std::back_inserter(probabilities), [](double x){return mpfr_class(x);});

	std::string message;
	mpfr_class result = 0;
	residualConnectivity::exhaustiveSearchUnequalProbabilities(*boostGraph.get(), probabilities, result, message);
	return Rcpp::wrap(result.str());
END_RCPP
}
