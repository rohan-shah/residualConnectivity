#ifndef PMC_R_PACKAGE_HEADER_GUARD
#define PMC_R_PACKAGE_HEADER_GUARD
#include <Rcpp.h>
SEXP PMC_igraph(SEXP graph_sexp, SEXP n_exp, SEXP seed_sexp);
SEXP PMC_graphNEL(SEXP graph_sexp, SEXP n_exp, SEXP seed_sexp);
SEXP PMC_graphAM(SEXP graph_sexp, SEXP n_exp, SEXP seed_sexp);
#endif