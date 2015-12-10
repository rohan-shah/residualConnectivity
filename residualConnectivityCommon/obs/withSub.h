#ifndef RESIDUAL_CONNECTIVITY_WITH_SUB_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_WITH_SUB_HEADER_GUARD
#include "observation.h"
#include "Context.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/shared_array.hpp>
namespace residualConnectivity
{
	class withSub : public ::residualConnectivity::observation
	{
	public:
		static void getSubObservation(int radius, vertexState* newState, const Context& context, const vertexState* oldStatesPtr);
	protected:
		withSub(Context const& context, boost::mt19937& randomSource);
		withSub(Context const& context, boost::shared_array<const vertexState> state);
		withSub(withSub&& other);
		withSub& operator=(withSub&& other);
		void getSubObservation(int radius, vertexState* outputState) const;
	};
}
#endif
