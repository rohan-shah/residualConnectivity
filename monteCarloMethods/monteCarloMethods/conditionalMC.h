#ifndef CONDITIONALMC_LIB_HEADER_GUARD
#define CONDITIONALMC_LIB_HEADER_GUARD
#include "context.h"
#include <boost/random/mersenne_twister.hpp>
namespace residualConnectivity
{
	struct conditionalMCArgs
	{
	public:
		conditionalMCArgs(const context& contextObj, boost::mt19937& randomSource)
			:contextObj(contextObj), randomSource(randomSource)
		{}
		const context& contextObj;
		boost::mt19937& randomSource;
		int n;
		mpfr_class estimate, expectedUpNumber;
	};
	void conditionalMC(conditionalMCArgs& args);
}
#endif
