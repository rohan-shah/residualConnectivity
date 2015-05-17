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
	{NULL, NULL, 0}
};
RcppExport void R_init_residualConnectivity(DllInfo *info)
{
	R_registerRoutines(info, NULL, callMethods, NULL, NULL);
}
