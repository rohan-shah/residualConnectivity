#ifndef RESIDUAL_CONNECTIVITY_SUB_OBS_ARTICULATION_CONDITIONING_IMPORTANCE_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_SUB_OBS_ARTICULATION_CONDITIONING_IMPORTANCE_HEADER_GUARD
#include "subObs/subObs.h"
#include "context.h"
#include "subObsTypes.h"
#include "subObs/getObservation.hpp"
#include "obs/getSubObservation.hpp"
#include "subObs/withWeight.h"
namespace residualConnectivity
{
	namespace obs
	{
		class articulationConditioningImportance;
	}
	namespace subObs
	{
		class articulationConditioningImportance : public ::residualConnectivity::subObs::withWeight
		{
		public:
			template<class T> friend class ::residualConnectivity::subObs::getObservation;
			template<class T> friend class ::residualConnectivity::obs::getSubObservation;

			typedef ::residualConnectivity::obs::articulationConditioningImportance observationType;
			typedef ::residualConnectivity::obs::withWeightImportanceConstructorType observationConstructorType;

			articulationConditioningImportance(articulationConditioningImportance&& other);
			bool isPotentiallyConnected() const;
			articulationConditioningImportance(context const& contextObj, boost::shared_array<vertexState> state, int radius, ::residualConnectivity::subObs::withWeightImportanceConstructorType &);
			void getObservation(vertexState* state, boost::mt19937& randomSource, observationConstructorType&) const;
			mpfr_class estimateRadius1(boost::mt19937& randomSource, int nSimulations, std::vector<int>& scratchMemory, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack);
			articulationConditioningImportance copyWithWeight(mpfr_class weight) const;
			articulationConditioningImportance(const articulationConditioningImportance& other);
			articulationConditioningImportance& operator=(const articulationConditioningImportance& other);
		protected:
			articulationConditioningImportance(const articulationConditioningImportance& other, mpfr_class weight);
		private:
			bool potentiallyConnected;
			const std::vector<double>* importanceProbabilities;
		};
	}
}
#endif
