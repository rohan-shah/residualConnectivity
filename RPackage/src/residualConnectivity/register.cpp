#include <Rcpp.h>
#include <internal.h>
#include "loadQt.h"
#include "crudeMC.h"
#include "conditionalMC.h"
#include "countConnectedSubgraphsBySize.h"
#include "stochasticEnumeration.h"
#include "createHexagonalLattice.h"
#include "articulationConditioningResampling.h"
#include "articulationConditioningSameCountResampling.h"
#include "articulationConditioningSameCountImportance.h"
#include "recursiveVarianceReduction.h"
#include "gridCountSpecificSize.h"
#include "optimalStateIndependentImportance.h"
#include "articulationConditioningSplitting.h"
#include "PMC.h"
#include "exactProbability.h"
#include "setDefaultPrec.h"
#include "averageComponentSize.h"
#ifdef _MSC_VER
	#undef RcppExport
	#define RcppExport extern "C" __declspec(dllexport)
#endif
extern "C" 
{
	const char* package_name = "residualConnectivity";
	R_CallMethodDef callMethods[] = 
	{
		{"loadQt", (DL_FUNC)&loadQt, 0},
		{"crudeMC", (DL_FUNC)&crudeMC, 4},
		{"recursiveVarianceReduction", (DL_FUNC)&recursiveVarianceReduction, 4},
		{"conditionalMC", (DL_FUNC)&conditionalMC, 4},
		{"countConnectedSubgraphsBySize", (DL_FUNC)&countConnectedSubgraphsBySize, 1},
		{"stochasticEnumeration", (DL_FUNC)&stochasticEnumeration, 7},
		{"articulationConditioningSameCountResampling", (DL_FUNC)&articulationConditioningSameCountResampling, 7},
		{"articulationConditioningResampling", (DL_FUNC)&articulationConditioningResampling, 7},
		{"articulationConditioningSameCountImportance", (DL_FUNC)&articulationConditioningSameCountImportance, 8},
		{"articulationConditioningSplitting", (DL_FUNC)&articulationConditioningSplitting, 7},
		{"PMC", (DL_FUNC)&PMC, 3},
		{"createHexagonalLattice", (DL_FUNC)&createHexagonalLattice, 2},
		{"gridCountSpecificSize", (DL_FUNC)&gridCountSpecificSize, 3},
		{"gridCountSpecificSize2", (DL_FUNC)&gridCountSpecificSize2, 2},
		{"optimalStateIndependentImportance", (DL_FUNC)&optimalStateIndependentImportance, 2},
		{"exactProbability", (DL_FUNC)&exactProbability, 2},
		{"setDefaultPrec", (DL_FUNC)&setDefaultPrec, 1}, 
		{"averageComponentSize", (DL_FUNC)&averageComponentSize, 4},
		{NULL, NULL, 0}
	};
	RcppExport void R_init_residualConnectivity(DllInfo *info)
	{
		std::vector<R_CallMethodDef> callMethodsVector;
		R_CallMethodDef* packageCallMethods = callMethods;
		while(packageCallMethods->name != NULL) packageCallMethods++;
		callMethodsVector.insert(callMethodsVector.begin(), callMethods, packageCallMethods);

		R_CallMethodDef* RcppStartCallMethods = Rcpp_get_call();
		R_CallMethodDef* RcppEndCallMethods = RcppStartCallMethods;
		while(RcppEndCallMethods->name != NULL) RcppEndCallMethods++;
		callMethodsVector.insert(callMethodsVector.end(), RcppStartCallMethods, RcppEndCallMethods);
		R_CallMethodDef blank = {NULL, NULL, 0};
		callMethodsVector.push_back(blank);

		R_registerRoutines(info, NULL, &(callMethodsVector[0]), NULL, NULL);
		init_Rcpp_cache();
	}
}
