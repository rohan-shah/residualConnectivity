#ifndef ARTICULATION_CONDITIONING_HEADER_GUARD
#define ARTICULATION_CONDITIONING_HEADER_GUARD
#include "context.h"
#include "observationTree.h"
#include "outputObject.h"
namespace residualConnectivity
{
	struct articulationConditioningArgs
	{
	public:
		articulationConditioningArgs(const context& contextObj, boost::mt19937& randomSource, outputObject& output)
		: contextObj(contextObj), randomSource(randomSource), output(output), initialRadius(-1), nLastStep(-1), n(-1), estimate(-1), verbose(true)
		{}
		const context& contextObj;
		boost::mt19937& randomSource;
		outputObject& output;
		int initialRadius;
		long nLastStep;
		int n;
		mpfr_class estimate;
		bool verbose;
	};
	void articulationConditioning(articulationConditioningArgs& args);
}
#endif
