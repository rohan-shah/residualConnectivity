#include <Rcpp.h>
#include <internal.h>
#include "loadQt.h"
#include "crudeMC.h"
#include "conditionalMC.h"
#include "countConnectedSubgraphs.h"
#include "stochasticEnumeration.h"
#include "createHexagonalLattice.h"
#include "articulationConditioningResampling.h"
#include "recursiveVarianceReduction.h"
#include "gridCountSpecificSize.h"
#include "optimalStateIndependentImportance.h"
#include "articulationConditioningSplitting.h"
#include "PMC.h"
#include "exhaustiveSearchUnequalProbabilities.h"
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
		{"crudeMC_igraph", (DL_FUNC)&crudeMC_igraph, 5},
		{"crudeMC_graphNEL", (DL_FUNC)&crudeMC_graphNEL, 5},
		{"crudeMC_graphAM", (DL_FUNC)&crudeMC_graphAM, 5},
		{"recursiveVarianceReduction_igraph", (DL_FUNC)&recursiveVarianceReduction_igraph, 5},
		{"recursiveVarianceReduction_graphNEL", (DL_FUNC)&recursiveVarianceReduction_graphNEL, 5},
		{"recursiveVarianceReduction_graphAM", (DL_FUNC)&recursiveVarianceReduction_graphAM, 5},
		{"conditionalMC_igraph", (DL_FUNC)&conditionalMC_igraph, 5},
		{"conditionalMC_graphNEL", (DL_FUNC)&conditionalMC_graphNEL, 5},
		{"conditionalMC_graphAM", (DL_FUNC)&conditionalMC_graphAM, 5},
		{"countConnectedSubgraphs_igraph", (DL_FUNC)&countConnectedSubgraphs_igraph, 1},
		{"countConnectedSubgraphs_graphNEL", (DL_FUNC)&countConnectedSubgraphs_graphNEL, 1},
		{"countConnectedSubgraphs_graphAM", (DL_FUNC)&countConnectedSubgraphs_graphAM, 1},
		{"stochasticEnumeration_igraph", (DL_FUNC)&stochasticEnumeration_igraph, 7},
		{"stochasticEnumeration_graphNEL", (DL_FUNC)&stochasticEnumeration_graphNEL, 7},
		{"stochasticEnumeration_graphAM", (DL_FUNC)&stochasticEnumeration_graphAM, 7},
		{"articulationConditioningResampling_igraph", (DL_FUNC)&articulationConditioningResampling_igraph, 6},
		{"articulationConditioningResampling_graphNEL", (DL_FUNC)&articulationConditioningResampling_graphNEL, 6},
		{"articulationConditioningResampling_graphAM", (DL_FUNC)&articulationConditioningResampling_graphAM, 6},
		{"articulationConditioningSplitting", (DL_FUNC)&articulationConditioningSplitting, 6},
		{"PMC_igraph", (DL_FUNC)&PMC_igraph, 3},
		{"PMC_graphNEL", (DL_FUNC)&PMC_graphNEL, 3},
		{"PMC_graphAM", (DL_FUNC)&PMC_graphAM, 3},
		{"createHexagonalLattice", (DL_FUNC)&createHexagonalLattice, 2},
		{"gridCountSpecificSize", (DL_FUNC)&gridCountSpecificSize, 3},
		{"gridCountSpecificSize2", (DL_FUNC)&gridCountSpecificSize2, 2},
		{"optimalStateIndependentImportance", (DL_FUNC)&optimalStateIndependentImportance, 2},
		{"exactRCRUnequalProbabilities", (DL_FUNC)&exhaustiveSearchUnequalProbabilities, 2},
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
