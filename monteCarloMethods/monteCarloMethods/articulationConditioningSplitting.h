#include <boost/random/mersenne_twister.hpp>
#include <vector>
#include "context.h"
#include "includeNumerics.h"
#include "outputObject.h"
namespace residualConnectivity
{
	struct articulationConditioningSplittingArgs
	{
		articulationConditioningSplittingArgs(const context& contextObj, boost::mt19937& randomSource, outputObject& output)
			:contextObj(contextObj), randomSource(randomSource), output(output)
		{}
		const context& contextObj;
		std::vector<double> splittingFactors;
		std::vector<double> levelProbabilities;
		int initialRadius;
		int n;
		boost::mt19937& randomSource;
		mpfr_class estimate;
		outputObject& output;
	};
	void articulationConditioningSplitting(articulationConditioningSplittingArgs& args);
}
