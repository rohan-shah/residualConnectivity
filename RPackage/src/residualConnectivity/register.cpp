#include <Rcpp.h>
#include "loadQt.h"
#include "crudeMCInterfaces.h"
#ifdef _MSC_VER
	#undef RcppExport
	#define RcppExport extern "C" __declspec(dllexport)
#endif
extern "C" char* package_name = "residualConnectivity";
R_CallMethodDef callMethods[] = 
{
	{"loadQT", (DL_FUNC)&loadQT, 0},
	{"crudeMC_igraph", (DL_FUNC)&crudeMC_igraph, 5},
	{"crudeMC_graphNEL", (DL_FUNC)&crudeMC_graphNEL, 4},
	{"crudeMC_graphAM", (DL_FUNC)&crudeMC_graphAM, 4},
	{NULL, NULL, 0}
};
extern "C" void R_init_Rcpp(DllInfo* info);
RcppExport void R_init_residualConnectivity(DllInfo *info)
{
	R_init_Rcpp(info);
	R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}
