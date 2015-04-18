#ifndef DISCRETE_GERM_GRAIN_SUB_OBS_ARTICULATION_CONDITIONING_FOR_SPLITTING_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_SUB_OBS_ARTICULATION_CONDITIONING_FOR_SPLITTING_HEADER_GUARD
#include "subObs/subObs.h"
#include "Context.h"
#include "subObsTypes.h"
#include "subObs/getObservation.hpp"
#include "obs/getSubObservation.hpp"
#include "subObs/withWeight.h"
namespace discreteGermGrain
{
	namespace obs
	{
		class articulationConditioningForSplitting;
	}
	namespace subObs
	{
		class articulationConditioningForSplitting : public ::discreteGermGrain::subObs::withWeight
		{
		public:
			template<class T> friend class ::discreteGermGrain::subObs::getObservation;
			template<class T> friend class ::discreteGermGrain::obs::getSubObservation;

			typedef ::discreteGermGrain::obs::articulationConditioningForSplitting observationType;
			typedef ::discreteGermGrain::obs::withWeightConstructorType observationConstructorType;

			articulationConditioningForSplitting(articulationConditioningForSplitting&& other);
			void estimateRadius1(boost::mt19937& randomSource, int nSimulations, std::vector<int>& scratchMemory, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack, std::vector<observationType>& outputObservations) const;
			bool isPotentiallyConnected() const;
			articulationConditioningForSplitting(Context const& context, boost::shared_array<vertexState> state, int radius, ::discreteGermGrain::subObs::withWeightConstructorType &);
			void getObservation(vertexState* state, boost::mt19937& randomSource, observationConstructorType&) const;
			articulationConditioningForSplitting copyWithWeight(mpfr_class weight) const;
		protected:
			articulationConditioningForSplitting(const articulationConditioningForSplitting& other, mpfr_class weight);
		private:
			articulationConditioningForSplitting(const articulationConditioningForSplitting& other);
			bool potentiallyConnected;
		};
	}
}
#endif
