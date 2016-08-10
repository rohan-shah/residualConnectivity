#ifndef ARTICULATION_CONDITIONING_RESAMPLING_HEADER_GUARD
#define ARTICULATION_CONDITIONING_RESAMPLING_HEADER_GUARD
#include "context.h"
#include "observationTree.h"
#include "outputObject.h"
namespace residualConnectivity
{
	struct articulationConditioningResamplingArgs
	{
	public:
		articulationConditioningResamplingArgs(const context& contextObj, boost::mt19937& randomSource, observationTree& tree, outputObject& output)
		: contextObj(contextObj), randomSource(randomSource), tree(tree), output(output)
		{}
		const context& contextObj;
		boost::mt19937& randomSource;
		observationTree& tree;
		outputObject& output;
		int initialRadius;
		int n;
		bool outputTree;
		mpfr_class expectedUpNumber, estimate;
	};
	void articulationConditioningResampling(articulationConditioningResamplingArgs& args);
}
#endif
