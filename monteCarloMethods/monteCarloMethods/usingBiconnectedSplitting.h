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
		usingBiconnectedSplittingArgs(const context& contextObj, outputObject& outputStream)
			:contextObj(contextObj), outputStream(outputStream)
		{}
		const context& contextObj;
		boost::mt19937 randomSource;
		int n;
		std::vector<float> splittingFactors;
		int initialRadius;
		mpfr_class estimate;
		outputObject& outputStream;
	};
	void usingBiconnectedSplitting(usingBiconnectedSplittingArgs& args);
}
#endif
