#include "subObs/usingMultipleLevelsConditioning.h"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/graph/biconnected_components.hpp>
#include "isSingleComponentWithRadius.h"
#include "subObs/usingMultipleLevelsConditioning.h"
#include "obs/usingMultipleLevelsConditioning.h"
#include "constructSubGraph.h"
#include "subObs/withFinalConditioning.hpp"
#include "conditionArticulation.h"
namespace discreteGermGrain
{
	namespace subObs
	{
		usingMultipleLevelsConditioning::usingMultipleLevelsConditioning(Context const& context, boost::shared_array<vertexState> state, int radius, ::discreteGermGrain::subObs::withWeightConstructorType& otherData)
			: ::discreteGermGrain::subObs::withWeight(context, state, radius, otherData.weight)
		{
			potentiallyConnected = isSingleComponentPossible(context, state.get(), otherData.components, otherData.stack);
			if(radius != 1 && potentiallyConnected)
			{
				conditionArticulation(state, weight, context, otherData.components, otherData.subGraphStack);
			}
		}
		bool usingMultipleLevelsConditioning::isPotentiallyConnected() const
		{
			return potentiallyConnected;
		}
		usingMultipleLevelsConditioning::usingMultipleLevelsConditioning(usingMultipleLevelsConditioning&& other)
			: ::discreteGermGrain::subObs::withWeight(static_cast<::discreteGermGrain::subObs::withWeight&&>(other)), potentiallyConnected(other.potentiallyConnected)
		{
			potentiallyConnected = other.potentiallyConnected;
		}
		void usingMultipleLevelsConditioning::getObservation(vertexState* outputState, boost::mt19937& randomSource, observationConstructorType& otherData)const
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
			otherData.weight = weight;
		}
		void usingMultipleLevelsConditioning::estimateRadius1(boost::mt19937& randomSource, int nSimulations, std::vector<int>& scratchMemory, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack, std::vector<observationType>& outputObservations) const
		{
			if(radius != 1)
			{
				throw std::runtime_error("Radius must be 1 to call constructRadius1Graph");
			}
			withFinalConditioning::estimateRadius1<usingMultipleLevelsConditioning>(*this, randomSource, nSimulations, scratchMemory, stack, outputObservations);
		}
		usingMultipleLevelsConditioning usingMultipleLevelsConditioning::copyWithWeight(mpfr_class weight) const
		{
			return usingMultipleLevelsConditioning(*this, weight);
		}
		usingMultipleLevelsConditioning::usingMultipleLevelsConditioning(const usingMultipleLevelsConditioning& other, mpfr_class weight)
			: ::discreteGermGrain::subObs::withWeight(static_cast<const ::discreteGermGrain::subObs::withWeight&>(other), weight), potentiallyConnected(other.potentiallyConnected)
		{}
		usingMultipleLevelsConditioning::usingMultipleLevelsConditioning(const usingMultipleLevelsConditioning& other)
			: ::discreteGermGrain::subObs::withWeight(static_cast<const ::discreteGermGrain::subObs::withWeight&>(other)), potentiallyConnected(other.potentiallyConnected)
		{}
	}
}
