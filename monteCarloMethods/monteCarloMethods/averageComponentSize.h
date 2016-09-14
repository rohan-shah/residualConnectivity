#ifndef AVERAGE_COMPONENT_SIZE_HEADER_GUARD
#define AVERAGE_COMPONENT_SIZE_HEADER_GUARD
#include "context.h"
#include <boost/random/mersenne_twister.hpp>
#include "isSingleComponentWithRadius.h"
namespace residualConnectivity
{
	struct averageComponentSizeArgs
	{
		averageComponentSizeArgs(const context& contextObj, std::vector<mpfr_class>& result, boost::mt19937& randomSource)
			:contextObj(contextObj), result(result), randomSource(randomSource)
		{}
		const context& contextObj;
		int n;
		std::vector<mpfr_class>& result;
		boost::mt19937& randomSource;
		std::vector<int> scratchMemory;
		boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType stack;
	};
	void averageComponentSize(averageComponentSizeArgs& args);
}
#endif
