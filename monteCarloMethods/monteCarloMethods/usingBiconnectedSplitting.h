#ifndef USING_BICONNECTED_SPLITTING_ARGS
#define USING_BICONNECTED_SPLITTING_ARGS
#include "context.h"
#include <vector>
#include "observationTree.h"
#include "includeMPFRResidualConnectivity.h"
#include <boost/random/mersenne_twister.hpp>
#include "outputObject.h"
namespace residualConnectivity
{
	struct usingBiconnectedSplittingArgs
	{
		usingBiconnectedSplittingArgs(const context& contextObj, observationTree& tree, outputObject& outputStream)
			:contextObj(contextObj), tree(tree), outputStream(outputStream)
		{}
		const context& contextObj;
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
