#ifndef ARTICULATION_CONDITIONING_RESAMPLING_HEADER_GUARD
#define ARTICULATION_CONDITIONING_RESAMPLING_HEADER_GUARD
#include "Context.h"
#include "observationTree.h"
#include "outputObject.h"
namespace discreteGermGrain
{
	struct articulationConditioningResamplingArgs
	{
	public:
		articulationConditioningResamplingArgs(const Context& context, boost::mt19937& randomSource, observationTree& tree, outputObject& output)
		: context(context), randomSource(randomSource), tree(tree), output(output)
		{}
		const Context& context;
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
