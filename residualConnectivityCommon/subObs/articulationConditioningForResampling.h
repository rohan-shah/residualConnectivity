#ifndef DISCRETE_GERM_GRAIN_SUB_OBS_ARTICULATION_CONDITIONING_FOR_RESAMPLING_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_SUB_OBS_ARTICULATION_CONDITIONING_FOR_RESAMPLING_HEADER_GUARD
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
		class articulationConditioningForResampling;
	}
	namespace subObs
	{
		class articulationConditioningForResampling : public ::discreteGermGrain::subObs::withWeight
		{
		public:
			template<class T> friend class ::discreteGermGrain::subObs::getObservation;
			template<class T> friend class ::discreteGermGrain::obs::getSubObservation;

			typedef ::discreteGermGrain::obs::articulationConditioningForResampling observationType;
			typedef ::discreteGermGrain::obs::withWeightConstructorType observationConstructorType;

			articulationConditioningForResampling(articulationConditioningForResampling&& other);
			bool isPotentiallyConnected() const;
			articulationConditioningForResampling(Context const& context, boost::shared_array<vertexState> state, int radius, ::discreteGermGrain::subObs::articulationConditioningForResamplingConstructorType &);
			void getObservation(vertexState* state, boost::mt19937& randomSource, observationConstructorType&) const;
			articulationConditioningForResampling copyWithWeight(mpfr_class weight) const;
		protected:
			articulationConditioningForResampling(const articulationConditioningForResampling& other, mpfr_class weight);
		private:
			articulationConditioningForResampling(const articulationConditioningForResampling& other);
			bool potentiallyConnected;
		};
	}
}
#endif
