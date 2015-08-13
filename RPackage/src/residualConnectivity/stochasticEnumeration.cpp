#include "stochasticEnumerationLib.h"
#include "Rcpp.h"
#include "stochasticEnumeration.h"
#include "graphInterface.h"
#include "graphConvert.h"
#include "calculateFactorials.h"
#include "graphType.h"
SEXP stochasticEnumeration(SEXP graph_sexp, SEXP optimized_sexp, SEXP budget_sexp, SEXP seed_sexp, SEXP counts_sexp, SEXP nPermutations_sexp, graphType type)
{
BEGIN_RCPP
	/*convert budget*/
	double budget_double;
	try
	{
		budget_double = Rcpp::as<double>(budget_sexp);
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Unable to convert input budget to a number");
	}
	long budget;
	if(std::abs(budget_double - std::round(budget_double)) > 1e-3)
	{
		throw std::runtime_error("Input budget must be an integer");
	}
	budget = (long)std::round(budget_double);

	bool optimized;
	try
	{
		optimized = Rcpp::as<bool>(optimized_sexp);
	}
	catch(Rcpp::not_compatible&)
	{
		throw std::runtime_error("Unable to convert input optimized to a boolean");
	}

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
	/*convert counts*/
	bool counts;
	try
	{
		counts = Rcpp::as<bool>(counts_sexp);
	}
	catch (Rcpp::not_compatible&)
	{
		throw std::runtime_error("Input counts must be a boolean");
	}
	/*Convert nPermutations*/
	int nPermutations;
	try
	{
		nPermutations = Rcpp::as<int>(nPermutations_sexp);
	}
	catch (Rcpp::not_compatible&)
	{
		throw std::runtime_error("Input nPermutations must be an integer");
	}
	if(nPermutations == NA_INTEGER)
	{
		throw std::runtime_error("Input nPermutations cannot be NA");
	}
	if(nPermutations < 1)
	{
		throw std::runtime_error("Input nPermutations must be at least 1");
	}

	boost::shared_ptr<discreteGermGrain::Context::inputGraph> graph = graphConvert(graph_sexp, type);
	const std::size_t nVertices = boost::num_vertices(*graph);
	boost::mt19937 randomSource;
	randomSource.seed(seed);

	std::vector<mpfr_class> estimatedCounts;
	discreteGermGrain::stochasticEnumerationArgs args(*graph, randomSource);
	args.n = budget;
	args.nPermutations = nPermutations;

	for(int i = 0; i < (int)nVertices+1; i++)
	{
		args.vertexCount = i;
		bool result;
		if(optimized) result = discreteGermGrain::stochasticEnumeration2(args);
		else result = discreteGermGrain::stochasticEnumeration1(args);
		if(result)
		{
			estimatedCounts.push_back(args.estimate);
		}
		else
		{
			throw std::runtime_error(args.message);
		}
	}

	if (counts)
	{
		/*Convert counts to strings*/
		std::vector<std::string> estimatedCountStrings;
		for (std::vector<mpfr_class>::iterator i = estimatedCounts.begin(); i != estimatedCounts.end(); i++)
		{
			estimatedCountStrings.push_back(i->str());
		}
		Rcpp::CharacterVector countStrings_sexp = Rcpp::wrap(estimatedCountStrings);
		Rcpp::Function mpfrFunction("mpfr");
		Rcpp::RObject result =  mpfrFunction(countStrings_sexp, Rcpp::Named("prec", 50));
		return result;
	}
	else
	{
		/*Divide by binomial coefficients*/
		std::vector<mpz_class> factorials;
		std::vector<mpfr_class> estimatedSpectra;
		discreteGermGrain::calculateFactorials(factorials, (int)nVertices + 1);
		for(int vertexCounter = 0; vertexCounter < (int)nVertices+1; vertexCounter++)
		{
			estimatedSpectra.push_back(mpfr_class((estimatedCounts[vertexCounter] * factorials[vertexCounter] * factorials[nVertices - vertexCounter]) / factorials[nVertices]));
		}
		/*Convert spectra to strings*/
		std::vector<std::string> estimatedSpectraStrings;
		for (std::vector<mpfr_class>::iterator i = estimatedSpectra.begin(); i != estimatedSpectra.end(); i++)
		{
			estimatedSpectraStrings.push_back(i->str());
		}
		Rcpp::CharacterVector spectraStrings_sexp = Rcpp::wrap(estimatedSpectraStrings);
		Rcpp::Function mpfrFunction("mpfr");
		Rcpp::RObject result =  mpfrFunction(spectraStrings_sexp, Rcpp::Named("prec", 50));
		return result;
	}
END_RCPP
}
SEXP stochasticEnumeration_igraph(SEXP graph_sexp, SEXP optimized_sexp, SEXP budget_sexp, SEXP seed_sexp, SEXP counts_sexp, SEXP nPermutations_sexp)
{
	return stochasticEnumeration(graph_sexp, optimized_sexp, budget_sexp, seed_sexp, counts_sexp, nPermutations_sexp, IGRAPH);
}
SEXP stochasticEnumeration_graphAM(SEXP graph_sexp, SEXP optimized_sexp, SEXP budget_sexp, SEXP seed_sexp, SEXP counts_sexp, SEXP nPermutations_sexp)
{
	return stochasticEnumeration(graph_sexp, optimized_sexp, budget_sexp, seed_sexp, counts_sexp, nPermutations_sexp, GRAPHAM);
}
SEXP stochasticEnumeration_graphNEL(SEXP graph_sexp, SEXP optimized_sexp, SEXP budget_sexp, SEXP seed_sexp, SEXP counts_sexp, SEXP nPermutations_sexp)
{
	return stochasticEnumeration(graph_sexp, optimized_sexp, budget_sexp, seed_sexp, counts_sexp, nPermutations_sexp, GRAPHNEL);
}