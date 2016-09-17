#ifndef RESIDUAL_CONNECTIVITY_WITH_SUB_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_WITH_SUB_HEADER_GUARD
#include "observation.h"
#include "context.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/shared_array.hpp>
namespace residualConnectivity
{
	class withSub : public ::residualConnectivity::observation
	{
	public:
		static void getSubObservation(int radius, vertexState* newState, const context& contextObj, const vertexState* oldStatesPtr);
	protected:
		withSub(context const& contextObj);
		withSub(context const& contextObj, boost::mt19937& randomSource);
		withSub(context const& contextObj, boost::shared_array<const vertexState> state);
		withSub(withSub&& other);
		withSub& operator=(withSub&& other);
		void getSubObservation(int radius, vertexState* outputState) const;
	};
}
#endif
