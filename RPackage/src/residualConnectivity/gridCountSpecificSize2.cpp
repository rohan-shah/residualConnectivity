#include "exactMethods/gridCountSpecificSize2.h"
#include "graphInterface.h"
#include "Rcpp.h"
SEXP gridCountSpecificSize2(SEXP gridDimension_sexp, SEXP size_sexp)
{
BEGIN_RCPP
	int gridDimension = Rcpp::as<int>(gridDimension_sexp);
	int size = Rcpp::as<int>(size_sexp);
	residualConnectivity::mpz_class count = residualConnectivity::gridCountSpecificSize2(gridDimension, size);
	return Rcpp::wrap(count.str());
END_RCPP
}
