#include "exactMethods/gridCountSpecificSize.h"
#include "graphInterface.h"
#include "Rcpp.h"
SEXP gridCountSpecificSize(SEXP gridDimension_sexp, SEXP size_sexp, SEXP multithreaded_sexp)
{
BEGIN_RCPP
	bool multithreaded = Rcpp::as<bool>(multithreaded_sexp);
	int gridDimension = Rcpp::as<int>(gridDimension_sexp);
	int size = Rcpp::as<int>(size_sexp);
	mpz_class count;
	if(multithreaded)
	{
		count = residualConnectivity::gridCountSpecificSizeMultiThreaded(gridDimension, size);
	}
	else
	{
		count = residualConnectivity::gridCountSpecificSizeSingleThreaded(gridDimension, size);
	}
	return Rcpp::wrap(count.str());
END_RCPP
}
