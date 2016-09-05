#include "setDefaultPrec.h"
#include "includeMPFRResidualConnectivity.h"
SEXP setDefaultPrec(SEXP prec_sexp)
{
BEGIN_RCPP
	residualConnectivity::mpfr_class::default_precision(Rcpp::as<int>(prec_sexp));
VOID_END_RCPP
	return R_NilValue;
}
