#ifndef STOCHASTIC_ENUMERATION_HEADER_GUARD
#define STOCHASTIC_ENUMERATION_HEADER_GUARD
#include <Rcpp.h>
SEXP stochasticEnumeration(SEXP graph_sexp, SEXP optimized_sexp, SEXP budget_sexp, SEXP seed_exp, SEXP counts_sexp, SEXP nPermutations_sexp, SEXP outputStatus_sexp);
#endif
