#include <Rcpp.h>
#include "loadQt.h"
#include "crudeMCInterfaces.h"
#include "conditionalMCInterfaces.h"
#include "exhaustiveSearchInterfaces.h"
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
	{"exhaustiveSearch_igraph", (DL_FUNC)&exhaustiveSearch_igraph, 1},
	{"exhaustiveSearch_graphNEL", (DL_FUNC)&exhaustiveSearch_graphNEL, 1},
	{"exhaustiveSearch_graphAM", (DL_FUNC)&exhaustiveSearch_graphAM, 1},
	{NULL, NULL, 0}
};
extern "C" void R_init_Rcpp(DllInfo* info);
RcppExport void R_init_residualConnectivity(DllInfo *info)
{
	R_init_Rcpp(info);
	R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}
