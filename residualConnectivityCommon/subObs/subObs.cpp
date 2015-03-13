#include "subObs/subObs.h"
#include <boost/random/bernoulli_distribution.hpp>
namespace discreteGermGrain
{
	namespace subObs
	{
		subObsWithRadius::subObsWithRadius(Context const& context, boost::shared_array<const vertexState> state, int radius)
			: ::discreteGermGrain::subObs::subObs(context, state), radius(radius)
		{}
		subObsWithRadius::subObsWithRadius(subObsWithRadius&& other)
			: ::discreteGermGrain::subObs::subObs(static_cast<::discreteGermGrain::subObs::subObs&&>(other))
		{
			radius = other.radius;
		}
		subObs::subObs(Context const& context, boost::shared_array<const vertexState> state)
			: ::discreteGermGrain::DiscreteGermGrainObs(context, state)
		{}
		subObs::subObs(subObs&& other)
			: ::discreteGermGrain::DiscreteGermGrainObs(static_cast<::discreteGermGrain::DiscreteGermGrainObs&&>(other))
		{}
		void subObs::getObservation(vertexState* outputState, boost::mt19937& randomSource, observationConstructorType&) const
		{
			boost::random::bernoulli_distribution<float> vertexDistribution(context.getOperationalProbabilityD());
			std::size_t nVertices = context.nVertices();
			memcpy(outputState, state.get(), sizeof(vertexState)*nVertices);
			//generate a full random grid, which includes the subPoints 
			for(std::size_t i = 0; i < nVertices; i++)
			{
				if(outputState[i].state & UNFIXED_MASK)
				{
					if(vertexDistribution(randomSource))
					{
						outputState[i].state = UNFIXED_ON;
					}
					else outputState[i].state = UNFIXED_OFF;
				}
			}
		}
	}
}
