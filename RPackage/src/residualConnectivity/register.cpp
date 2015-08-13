#include <Rcpp.h>
#include "loadQt.h"
#include "crudeMC.h"
#include "conditionalMC.h"
#include "countConnectedSubgraphs.h"
#include "stochasticEnumeration.h"
#include "PMC.h"
#ifdef _MSC_VER
	#undef RcppExport
	#define RcppExport extern "C" __declspec(dllexport)
#endif
extern "C" const char* package_name = "residualConnectivity";
R_CallMethodDef callMethods[] = 
{
	{"loadQt", (DL_FUNC)&loadQt, 0},
	{"crudeMC_igraph", (DL_FUNC)&crudeMC_igraph, 5},
	{"crudeMC_graphNEL", (DL_FUNC)&crudeMC_graphNEL, 5},
	{"crudeMC_graphAM", (DL_FUNC)&crudeMC_graphAM, 5},
	{"conditionalMC_igraph", (DL_FUNC)&conditionalMC_igraph, 5},
	{"conditionalMC_graphNEL", (DL_FUNC)&conditionalMC_graphNEL, 5},
	{"conditionalMC_graphAM", (DL_FUNC)&conditionalMC_graphAM, 5},
	{"countConnectedSubgraphs_igraph", (DL_FUNC)&countConnectedSubgraphs_igraph, 1},
	{"countConnectedSubgraphs_graphNEL", (DL_FUNC)&countConnectedSubgraphs_graphNEL, 1},
	{"countConnectedSubgraphs_graphAM", (DL_FUNC)&countConnectedSubgraphs_graphAM, 1},
	{"stochasticEnumeration_igraph", (DL_FUNC)&stochasticEnumeration_igraph, 6},
	{"stochasticEnumeration_graphNEL", (DL_FUNC)&stochasticEnumeration_graphNEL, 6},
	{"stochasticEnumeration_graphAM", (DL_FUNC)&stochasticEnumeration_graphAM, 6},
	{"PMC_igraph", (DL_FUNC)&PMC_igraph, 3},
	{"PMC_graphNEL", (DL_FUNC)&PMC_graphNEL, 3},
	{"PMC_graphAM", (DL_FUNC)&PMC_graphAM, 3},
	{NULL, NULL, 0}
};
extern "C" void R_init_Rcpp(DllInfo* info);
RcppExport void R_init_residualConnectivity(DllInfo *info)
{
	R_init_Rcpp(info);
	R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}
