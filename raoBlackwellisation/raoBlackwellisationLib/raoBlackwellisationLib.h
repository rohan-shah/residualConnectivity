#ifndef RAO_BLACKWELLISATION_LIB_HEADER_GUARD
#define RAO_BLACKWELLISATION_LIB_HEADER_GUARD
#include "Context.h"
#include <boost/random/mersenne_twister.hpp>
namespace discreteGermGrain
{
	struct raoBlackwellisationArgs
	{
	public:
		raoBlackwellisationArgs(const Context& context, boost::mt19937& randomSource)
			:context(context), randomSource(randomSource)
		{}
		const Context& context;
		boost::mt19937& randomSource;
		int n;
		mpfr_class probability;
		mpfr_class estimate, expectedUpNumber;
	};
	void raoBlackwellisation(raoBlackwellisationArgs& args);
}
#endif