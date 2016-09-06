#ifndef CRUDE_MC_LIB_HEADER_GUARD
#define CRUDE_MC_LIB_HEADER_GUARD
#include "context.h"
#include <boost/random/mersenne_twister.hpp>
namespace residualConnectivity
{
	struct crudeMCArgs
	{
	public:
		crudeMCArgs(const context& contextObj, boost::mt19937& randomSource)
			:contextObj(contextObj), randomSource(randomSource)
		{}
		int n;
		const context& contextObj;
		boost::mt19937& randomSource;
	};
	std::size_t crudeMC(crudeMCArgs& args);
}
#endif