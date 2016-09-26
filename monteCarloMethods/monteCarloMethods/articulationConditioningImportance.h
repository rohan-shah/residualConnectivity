#ifndef ARTICULATION_CONDITIONING_IMPORTANCE_HEADER_GUARD
#define ARTICULATION_CONDITIONING_IMPORTANCE_HEADER_GUARD
#include "context.h"
#include "observationTree.h"
#include "outputObject.h"
namespace residualConnectivity
{
	struct articulationConditioningImportanceArgs
	{
	public:
		articulationConditioningImportanceArgs(const context& contextObj, boost::mt19937& randomSource, outputObject& output)
		: contextObj(contextObj), randomSource(randomSource), output(output)
		{}
		const context& contextObj;
		boost::mt19937& randomSource;
		outputObject& output;
		int initialRadius;
		int n;
		int finalStepSampleSize;
		mpfr_class estimate;
		bool verbose;
		std::vector<double> importanceProbabilities;
	};
	void articulationConditioningImportance(articulationConditioningImportanceArgs& args);
}
#endif
