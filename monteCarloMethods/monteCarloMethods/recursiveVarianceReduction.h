#ifndef RECURSIVE_VARIANCE_REDUCTION_HEADER_GUARD
#define RECURSIVE_VARIANCE_REDUCTION_HEADER_GUARD
#include "context.h"
#include <boost/random/mersenne_twister.hpp>
namespace residualConnectivity
{
	struct recursiveVarianceReductionArgs
	{
	public:
		recursiveVarianceReductionArgs(const context& contextObj, boost::mt19937& randomSource)
		: contextObj(contextObj), randomSource(randomSource)
		{}
		const context& contextObj;
		boost::mt19937& randomSource;
		int n;
		mpfr_class estimate;
	};
	void recursiveVarianceReduction(recursiveVarianceReductionArgs& args);
}
#endif
