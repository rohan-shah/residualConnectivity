#include "PMC.h"
#include "monteCarloMethods/PMC.h"
#include "graphType.h"
#include "graphConvert.h"
SEXP PMC(SEXP graph_sexp, SEXP n_sexp, SEXP seed_sexp, graphType type)
{
BEGIN_RCPP
	boost::shared_ptr<residualConnectivity::Context::inputGraph> graph = graphConvert(graph_sexp, type);
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
		mpfr_class spectraValue = mpfr_class(*i) / n;
		ss << spectraValue.str();
		spectraStrings.push_back(ss.str());
	}
	Rcpp::CharacterVector spectraStrings_sexp = Rcpp::wrap(spectraStrings);
	Rcpp::Function mpfrFunction("mpfr");
	Rcpp::RObject result = mpfrFunction(spectraStrings_sexp, Rcpp::Named("prec", 50));
	return result;
END_RCPP
}
SEXP PMC_igraph(SEXP graph_sexp, SEXP n_exp, SEXP seed_sexp)
{
	return PMC(graph_sexp, n_exp, seed_sexp, IGRAPH);
}
SEXP PMC_graphNEL(SEXP graph_sexp, SEXP n_exp, SEXP seed_sexp)
{
	return PMC(graph_sexp, n_exp, seed_sexp, GRAPHNEL);
}
SEXP PMC_graphAM(SEXP graph_sexp, SEXP n_exp, SEXP seed_sexp)
{
	return PMC(graph_sexp, n_exp, seed_sexp, GRAPHAM);
}
