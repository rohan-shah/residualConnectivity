#ifndef AVERAGE_COMPONENT_SIZE_R_PACKAGE_HEADER_GUARD
#define AVERAGE_COMPONENT_SIZE_R_PACKAGE_HEADER_GUARD
#include <Rcpp.h>
SEXP averageComponentSize(SEXP graph_sexp, SEXP probabilities_sexp, SEXP n_sexp, SEXP seed_sexp);
#endif
