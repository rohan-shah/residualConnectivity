#include "optimalStateIndependentImportance.h"
#include "includeNumerics.h"
#include "exactMethods/optimalStateIndependentImportance.h"
SEXP optimalStateIndependentImportance(SEXP exactRCR_sexp, SEXP probabilities_sexp)
{
BEGIN_RCPP
	Rcpp::S4 exactRCR = Rcpp::as<Rcpp::S4>(exactRCR_sexp);
	Rcpp::Function asCharacter("as.character");
	std::vector<std::string> exactCountsString = Rcpp::as<std::vector<std::string> >(asCharacter(exactRCR.slot("counts")));
	std::vector<mpz_class> exactCounts;
	std::transform(exactCountsString.begin(), exactCountsString.end(), std::back_inserter(exactCounts), [](std::string x){return mpz_class(x);});

	Rcpp::NumericVector probabilities_double = Rcpp::as<Rcpp::NumericVector>(probabilities_sexp);
	std::vector<mpfr_class> probabilities;
	std::transform(probabilities_double.begin(), probabilities_double.end(), std::back_inserter(probabilities), [](double x){return mpfr_class(x);});
	std::vector<mpfr_class> bestImportanceProbabilities;
	residualConnectivity::optimalStateIndependentImportance(exactCounts, probabilities, bestImportanceProbabilities);

	std::vector<double> retVal;
	std::transform(bestImportanceProbabilities.begin(), bestImportanceProbabilities.end(), std::back_inserter(retVal), [](mpfr_class x){return x.convert_to<double>();});
	return Rcpp::wrap(retVal);
END_RCPP
}
