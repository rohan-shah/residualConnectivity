#ifndef RESIDUAL_CONNECTIVITY_SUB_OBS_ARTICULATION_CONDITIONING_SAME_COUNT_IMPORTANCE_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_SUB_OBS_ARTICULATION_CONDITIONING_SAME_COUNT_IMPORTANCE_HEADER_GUARD
#include "subObs/subObs.h"
#include "context.h"
#include "subObsTypes.h"
#include "subObs/getObservation.hpp"
#include "obs/getSubObservation.hpp"
#include "subObs/withWeight.h"
#include "depth_first_search_restricted.hpp"
#include "conditionArticulation.h"
#include "conditionalPoissonBase.h"
#include "samplingBase.h"
namespace residualConnectivity
{
	namespace obs
	{
		class articulationConditioningSameCountImportance;
	}
	namespace subObs
	{
		class articulationConditioningSameCountImportance : public ::residualConnectivity::subObs::withWeight
		{
		public:
			template<class T> friend class ::residualConnectivity::subObs::getObservation;
			template<class T> friend class ::residualConnectivity::obs::getSubObservation;

			typedef ::residualConnectivity::obs::articulationConditioningSameCountImportance observationType;
			typedef ::residualConnectivity::obs::articulationConditioningSameCountImportanceConstructorType observationConstructorType;

			articulationConditioningSameCountImportance(articulationConditioningSameCountImportance&& other);
			bool isPotentiallyConnected() const;
			articulationConditioningSameCountImportance(context const& contextObj, boost::shared_array<vertexState> state, int radius, ::residualConnectivity::subObs::articulationConditioningSameCountImportanceConstructorType &);
			void getObservation(vertexState* state, boost::mt19937& randomSource, observationConstructorType&) const;
			articulationConditioningSameCountImportance copyWithWeight(mpfr_class weight) const;
			mpfr_class estimateRadius1(boost::mt19937& randomSource, int nSimulations, std::vector<int>& scratchMemory, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack) const;
		protected:
			articulationConditioningSameCountImportance(const articulationConditioningSameCountImportance& other, mpfr_class weight, int nUpVertices, const std::vector<double>* importanceProbabilities);
		private:
			void conditionArticulation(boost::shared_array<vertexState> state, std::vector<int>& scratch, boost::detail::depth_first_visit_restricted_impl_helper<filteredGraphType>::stackType& filteredGraphStack);
			std::vector<int> simulationOrder;
			articulationConditioningSameCountImportance(const articulationConditioningSameCountImportance& other);
			bool potentiallyConnected;
			int nUpVertices;
			sampling::conditionalPoissonArgs samplingArgsOriginal, samplingArgsImportance;
			int nConditioningVertices;
			const std::vector<double>* importanceProbabilities;
		};
	}
}
#endif
