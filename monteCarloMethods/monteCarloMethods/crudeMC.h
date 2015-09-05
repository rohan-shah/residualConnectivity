#ifndef CRUDE_MC_LIB_HEADER_GUARD
#define CRUDE_MC_LIB_HEADER_GUARD
#include "Context.h"
#include <boost/random/mersenne_twister.hpp>
namespace discreteGermGrain
{
	struct crudeMCArgs
	{
	public:
		crudeMCArgs(const Context& context, boost::mt19937& randomSource)
			:context(context), randomSource(randomSource)
		{}
		int n;
		const Context& context;
		boost::mt19937& randomSource;
	};
	std::size_t crudeMC(crudeMCArgs& args);
}
#endif