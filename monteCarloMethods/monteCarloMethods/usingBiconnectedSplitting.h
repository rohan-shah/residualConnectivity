#ifndef USING_BICONNECTED_SPLITTING_ARGS
#define USING_BICONNECTED_SPLITTING_ARGS
#include "Context.h"
#include <vector>
#include "observationTree.h"
#include "includeNumerics.h"
#include <boost/random/mersenne_twister.hpp>
#include "outputObject.h"
namespace residualConnectivity
{
	struct usingBiconnectedSplittingArgs
	{
		usingBiconnectedSplittingArgs(const Context& context, observationTree& tree, outputObject& outputStream)
			:context(context), tree(tree), outputStream(outputStream)
		{}
		const Context& context;
		boost::mt19937 randomSource;
		int n;
		std::vector<float> splittingFactors;
		observationTree& tree;
		int initialRadius;
		mpfr_class estimate;
		std::string outputDistributionFile, outputTreeFile;
		bool outputDistribution;
		bool outputTree;
		outputObject& outputStream;
	};
	void usingBiconnectedSplitting(usingBiconnectedSplittingArgs& args);
}
#endif
