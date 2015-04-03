#ifndef DISCRETE_GERM_GRAIN_WITH_SUB_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_WITH_SUB_HEADER_GUARD
#include "observation.h"
#include "Context.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/shared_array.hpp>
namespace discreteGermGrain
{
	class withSub : public ::discreteGermGrain::observation
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
