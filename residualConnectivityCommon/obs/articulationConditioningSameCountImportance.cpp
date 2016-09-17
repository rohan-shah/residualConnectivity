#include "obs/articulationConditioningSameCountImportance.h"
#include <boost/random/bernoulli_distribution.hpp>
#include "conditionalPoissonBase.h"
#include "samplingBase.h"
namespace residualConnectivity
{
	namespace obs
	{
		articulationConditioningSameCountImportance::articulationConditioningSameCountImportance(context const& contextObj, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::articulationConditioningSameCountImportanceConstructorType& otherData)
			: ::residualConnectivity::withSub(contextObj, state), weight(otherData.weight), nUpVertices(otherData.count), importanceProbabilities(otherData.importanceProbabilities)
		{}
		articulationConditioningSameCountImportance& articulationConditioningSameCountImportance::operator=(articulationConditioningSameCountImportance&& other)
		{
			weight = other.weight;
			nUpVertices = other.nUpVertices;
			importanceProbabilities = other.importanceProbabilities;
			state = other.state;
			return *this;
		}
		articulationConditioningSameCountImportance::articulationConditioningSameCountImportance(context const& contextObj, const std::vector<double>* importanceProbabilities, boost::mt19937& randomSource)
			: ::residualConnectivity::withSub(contextObj), weight(1), importanceProbabilities(importanceProbabilities)
		{
			nUpVertices = 0;
			std::size_t nVertices = boost::num_vertices(contextObj.getGraph());
			boost::shared_array<vertexState> state(new vertexState[nVertices]);

			for(std::size_t i = 0; i < nVertices; i++)
			{
				boost::random::bernoulli_distribution<double> vertexDistribution((*importanceProbabilities)[i]);
				if(vertexDistribution(randomSource))
				{
					state[i].state = UNFIXED_ON;
					nUpVertices++;
				}
				else
				{
					state[i].state = UNFIXED_OFF;
				}
			}
			this->state = state;
		}
		void articulationConditioningSameCountImportance::getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const
		{
			const std::vector<double>& operationalProbabilities = contextObj.getOperationalProbabilitiesD();
			::residualConnectivity::withSub::getSubObservation(radius, newState);
			//The indices that have just been fixed. 
			std::vector<int> samplingOrder;
			std::size_t nVertices = boost::num_vertices(contextObj.getGraph());
			for(std::size_t i = 0; i < nVertices; i++)
			{
				if(newState[i].state != state[i].state && (newState[i].state & FIXED_MASK))
				{
					samplingOrder.push_back((int)i);
				}
			}
			int nImportance = (int)samplingOrder.size();
			for(std::size_t i = 0; i < nVertices; i++)
			{
				if(newState[i].state == state[i].state || !(newState[i].state & FIXED_MASK))
				{
					samplingOrder.push_back((int)i);
				}
			}
			sampling::conditionalPoissonArgs samplingArgsOriginal, samplingArgsImportance;
			samplingArgsOriginal.n = samplingArgsImportance.n = nUpVertices;

			for(int i = 0; i < nImportance; i++)
			{
				samplingArgsOriginal.weights.push_back(operationalProbabilities[samplingOrder[i]]);
				samplingArgsImportance.weights.push_back((*importanceProbabilities)[samplingOrder[i]]);
			}
			for(int i = nImportance; i < (int)nVertices; i++)
			{
				if(newState[samplingOrder[i]].state == FIXED_ON)
				{
					samplingArgsOriginal.weights.push_back(1);
					samplingArgsImportance.weights.push_back(1);
				}
				else if(newState[samplingOrder[i]].state == FIXED_OFF)
				{
					samplingArgsOriginal.weights.push_back(0);
					samplingArgsImportance.weights.push_back(0);
				}
				else
				{
					samplingArgsOriginal.weights.push_back(operationalProbabilities[samplingOrder[i]]);
					samplingArgsImportance.weights.push_back((*importanceProbabilities)[samplingOrder[i]]);
				}
			}
			int nDeterministic = 0, nZeroWeights = 0;
			sampling::samplingBase(samplingArgsOriginal.n, samplingArgsOriginal.indices, samplingArgsOriginal.weights, samplingArgsOriginal.zeroWeights, samplingArgsOriginal.deterministicInclusion, nDeterministic, nZeroWeights);
			sampling::computeExponentialParameters(samplingArgsOriginal);
			calculateExpNormalisingConstants(samplingArgsOriginal);

			nDeterministic = 0, nZeroWeights = 0;
			sampling::samplingBase(samplingArgsImportance.n, samplingArgsImportance.indices, samplingArgsImportance.weights, samplingArgsImportance.zeroWeights, samplingArgsImportance.deterministicInclusion, nDeterministic, nZeroWeights);
			sampling::computeExponentialParameters(samplingArgsImportance);
			calculateExpNormalisingConstants(samplingArgsImportance);

			mpfr_class newWeight = 1;
			int chosen = 0, skipped = 0;
			//In the case that nDeterministic == samplingArgsOriginal.n, all the newly fixed vertices must be off, and this occurs with probability one under both the original and importance densities
			if(nDeterministic != (int)samplingArgsOriginal.n)
			{
				for(int i = 0; i < nImportance; i++)
				{
					if(!samplingArgsOriginal.zeroWeights[i] && !samplingArgsOriginal.deterministicInclusion[i])
					{
						mpfr_class originalParameter, importanceParameter;
						if(samplingArgsOriginal.n - nDeterministic - 1 - chosen == 0)
						{
							originalParameter = (samplingArgsOriginal.expExponentialParameters[i] / samplingArgsOriginal.expNormalisingConstant(i-skipped, samplingArgsOriginal.n - nDeterministic - chosen - 1));
							importanceParameter = (samplingArgsImportance.expExponentialParameters[i] / samplingArgsImportance.expNormalisingConstant(i-skipped, samplingArgsImportance.n - nDeterministic - chosen - 1));
						}
						else
						{
							originalParameter = ((samplingArgsOriginal.expExponentialParameters[i] * samplingArgsOriginal.expNormalisingConstant(i+1-skipped, samplingArgsOriginal.n - nDeterministic - 1 - chosen - 1) / samplingArgsOriginal.expNormalisingConstant(i-skipped, samplingArgsOriginal.n - nDeterministic - chosen - 1)));
							importanceParameter = ((samplingArgsImportance.expExponentialParameters[i] * samplingArgsImportance.expNormalisingConstant(i+1-skipped, samplingArgsImportance.n - nDeterministic - 1 - chosen - 1) / samplingArgsImportance.expNormalisingConstant(i-skipped, samplingArgsImportance.n - nDeterministic - chosen - 1)));
						}
						if(newState[samplingOrder[i]].state == FIXED_ON)
						{
							newWeight *= originalParameter / importanceParameter;
							chosen++;
						}
						else
						{
							newWeight *= (1 - originalParameter) / (1 - importanceParameter);
						}
					}
					else skipped++;
					if(samplingArgsOriginal.n - nDeterministic - chosen == 0) break;
				}
			}
			other.importanceProbabilities = importanceProbabilities;
			other.weight = weight * newWeight;
			other.count = nUpVertices;
		}
		articulationConditioningSameCountImportance::articulationConditioningSameCountImportance(articulationConditioningSameCountImportance&& other)
			: ::residualConnectivity::withSub(static_cast<::residualConnectivity::withSub&&>(other)), weight(other.weight), nUpVertices(other.nUpVertices), importanceProbabilities(other.importanceProbabilities)
		{}
		const mpfr_class& articulationConditioningSameCountImportance::getWeight() const
		{
			return weight;
		}
	}
}
