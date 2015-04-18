#ifndef DISCRETE_GERM_GRAIN_ARTICULATION_CONDITIONING_FOR_SPLITTING_OBS_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_ARTICULATION_CONDITIONING_FOR_SPLITTING_OBS_HEADER_GUARD
#include "subObsTypes.h"
#include "obs/getSubObservation.hpp"
#include "subObs/getObservation.hpp"
#include "obs/withSub.h"
#include <boost/shared_array.hpp>
#include "Context.h"
#include <boost/random/mersenne_twister.hpp>
#include "subObsTypes.h"
namespace discreteGermGrain
{
	namespace subObs
	{
		class articulationConditioningForSplitting;
	}
	namespace obs
	{
		class articulationConditioningForSplitting : public ::discreteGermGrain::withSub
		{
		public:
			template<class T> friend class ::discreteGermGrain::obs::getSubObservation;
			template<class T> friend class ::discreteGermGrain::subObs::getObservation;

			typedef ::discreteGermGrain::subObs::articulationConditioningForSplitting subObservationType;
			typedef ::discreteGermGrain::subObs::withWeightConstructorType subObservationConstructorType;

			articulationConditioningForSplitting(Context const& context, boost::mt19937& randomSource);
			articulationConditioningForSplitting(articulationConditioningForSplitting&& other);
			articulationConditioningForSplitting& operator=(articulationConditioningForSplitting&& other);
			articulationConditioningForSplitting(Context const& context, boost::shared_array<const vertexState> state, ::discreteGermGrain::obs::withWeightConstructorType&);
			const mpfr_class& getWeight() const;
		private:
			void getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const;
			mpfr_class weight;
		};
	}
}
#endif
