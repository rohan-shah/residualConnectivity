#ifndef RESIDUAL_CONNECTIVITY_SUB_OBS_ARTICULATION_CONDITIONING_FOR_RESAMPLING_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_SUB_OBS_ARTICULATION_CONDITIONING_FOR_RESAMPLING_HEADER_GUARD
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
		class articulationConditioningForResampling;
	}
	namespace subObs
	{
		class articulationConditioningForResampling : public ::residualConnectivity::subObs::withWeight
		{
		public:
			template<class T> friend class ::residualConnectivity::subObs::getObservation;
			template<class T> friend class ::residualConnectivity::obs::getSubObservation;

			typedef ::residualConnectivity::obs::articulationConditioningForResampling observationType;
			typedef ::residualConnectivity::obs::withWeightConstructorType observationConstructorType;

			articulationConditioningForResampling(articulationConditioningForResampling&& other);
			bool isPotentiallyConnected() const;
			articulationConditioningForResampling(context const& contextObj, boost::shared_array<vertexState> state, int radius, ::residualConnectivity::subObs::articulationConditioningForResamplingConstructorType &);
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
