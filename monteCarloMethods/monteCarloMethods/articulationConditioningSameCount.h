#ifndef ARTICULATION_CONDITIONING_SAME_COUNT_HEADER_GUARD
#define ARTICULATION_CONDITIONING_SAME_COUNT_HEADER_GUARD
#include "context.h"
#include "observationTree.h"
#include "outputObject.h"
namespace residualConnectivity
{
	struct articulationConditioningSameCountArgs
	{
	public:
		articulationConditioningSameCountArgs(const context& contextObj, boost::mt19937& randomSource, outputObject& output)
		: contextObj(contextObj), randomSource(randomSource), output(output)
		{}
		const context& contextObj;
		boost::mt19937& randomSource;
		outputObject& output;
		int initialRadius;
		int n;
		mpfr_class expectedUpNumber, estimate;
		bool verbose;
	};
	void articulationConditioningSameCount(articulationConditioningSameCountArgs& args);
}
#endif
