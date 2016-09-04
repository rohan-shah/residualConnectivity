#include "PMC.h"
#include "monteCarloMethods/PMC.h"
#include "graphConvert.h"
SEXP PMC(SEXP graph_sexp, SEXP n_sexp, SEXP seed_sexp)
{
BEGIN_RCPP
	boost::shared_ptr<residualConnectivity::context::inputGraph> graph(new residualConnectivity::context::inputGraph());
	std::vector<residualConnectivity::context::vertexPosition> vertexCoordinates;
	graphConvert(graph_sexp, *graph.get(), vertexCoordinates);
	boost::mt19937 randomSource;

	/*convert seed*/
	int seed;
	try
	{
		seed = Rcpp::as<int>(seed_sexp);
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Input seed must be an integer");
	}

	/*convert n*/
	int n;
	try
	{
		n = Rcpp::as<int>(n_sexp);
	}
	catch (Rcpp::not_compatible&)
	{
		throw std::runtime_error("Input seed must be an integer");
	}

	randomSource.seed(seed);
	residualConnectivity::pmcArguments arguments(*graph, randomSource);
	arguments.n = n;

	residualConnectivity::PMC(arguments);

	std::vector<std::string> spectraStrings;
	for (std::vector<unsigned long long>::iterator i = arguments.counts.begin(); i != arguments.counts.end(); i++)
	{
		std::stringstream ss;
		residualConnectivity::mpfr_class spectraValue = residualConnectivity::mpfr_class(*i) / n;
		ss << spectraValue.str();
		spectraStrings.push_back(ss.str());
	}
	Rcpp::CharacterVector spectraStrings_sexp = Rcpp::wrap(spectraStrings);
	Rcpp::Function mpfrFunction("mpfr");
	Rcpp::RObject result = mpfrFunction(spectraStrings_sexp, Rcpp::Named("prec", 50));
	return result;
END_RCPP
}
