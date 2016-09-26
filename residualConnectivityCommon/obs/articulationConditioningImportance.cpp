#include "obs/articulationConditioningImportance.h"
#include <boost/random/bernoulli_distribution.hpp>
namespace residualConnectivity
{
	namespace obs
	{
		articulationConditioningImportance::articulationConditioningImportance(context const& contextObj, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::withWeightImportanceConstructorType& otherData)
			: ::residualConnectivity::withSub(contextObj, state), weight(otherData.weight), importanceProbabilities(otherData.importanceProbabilities)
		{}
		articulationConditioningImportance::articulationConditioningImportance(context const& contextObj, boost::mt19937& randomSource, const std::vector<double>* importanceProbabilities)
			: ::residualConnectivity::withSub(contextObj), weight(1), importanceProbabilities(importanceProbabilities)
		{
			std::size_t nVertices = boost::num_vertices(contextObj.getGraph());
			boost::shared_array<vertexState> state(new vertexState[nVertices]);

			for(std::size_t i = 0; i < nVertices; i++)
			{
				boost::random::bernoulli_distribution<double> vertexDistribution((*importanceProbabilities)[i]);
				if(vertexDistribution(randomSource))
				{
					state[i].state = UNFIXED_ON;
				}
				else
				{
					state[i].state = UNFIXED_OFF;
				}
			}
			this->state = state;
		}
		void articulationConditioningImportance::getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const
		{
			::residualConnectivity::withSub::getSubObservation(radius, newState);
			std::size_t nVertices = boost::num_vertices(contextObj.getGraph());
			const std::vector<mpfr_class>& opProbabilities = contextObj.getOperationalProbabilities();
			mpfr_class newWeight = 1;
			for(std::size_t i = 0; i < nVertices; i++)
			{
				if(newState[i].state != state[i].state && (newState[i].state & FIXED_MASK))
				{
					if(newState[i].state == FIXED_ON)
					{
						newWeight *= opProbabilities[i] / (*importanceProbabilities)[i];
					}
					else
					{
						newWeight *= (1 - opProbabilities[i]) / (1 - (*importanceProbabilities)[i]);
					}
				}
			}
			other.weight = newWeight * weight;
			other.importanceProbabilities = importanceProbabilities;
		}
		articulationConditioningImportance::articulationConditioningImportance(articulationConditioningImportance&& other)
			: ::residualConnectivity::withSub(static_cast< ::residualConnectivity::withSub&&>(other)), weight(other.weight), importanceProbabilities(other.importanceProbabilities)
		{}
		const mpfr_class& articulationConditioningImportance::getWeight() const
		{
			return weight;
		}
	}
}
