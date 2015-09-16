#ifndef RECURSIVE_VARIANCE_REDUCTION_HEADER_GUARD
#define RECURSIVE_VARIANCE_REDUCTION_HEADER_GUARD
#include "Context.h"
#include <boost/random/mersenne_twister.hpp>
namespace residualConnectivity
{
	struct recursiveVarianceReductionArgs
	{
	public:
		recursiveVarianceReductionArgs(const Context& context, boost::mt19937& randomSource)
		: context(context), randomSource(randomSource)
		{}
		const Context& context;
		boost::mt19937& randomSource;
		int n;
		mpfr_class estimate;
	};
	void recursiveVarianceReduction(recursiveVarianceReductionArgs& args);
}
#endif
