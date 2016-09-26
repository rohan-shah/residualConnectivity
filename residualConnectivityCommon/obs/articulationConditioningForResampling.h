#ifndef RESIDUAL_CONNECTIVITY_ARTICULATION_CONDITIONING_FOR_RESAMPLING_OBS_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_ARTICULATION_CONDITIONING_FOR_RESAMPLING_OBS_HEADER_GUARD
#include "subObsTypes.h"
#include "obs/getSubObservation.hpp"
#include "subObs/getObservation.hpp"
#include "obs/withSub.h"
#include <boost/shared_array.hpp>
#include "context.h"
#include <boost/random/mersenne_twister.hpp>
#include "subObsTypes.h"
namespace residualConnectivity
{
	namespace subObs
	{
		class articulationConditioningForResampling;
	}
	namespace obs
	{
		class articulationConditioningForResampling : public ::residualConnectivity::withSub
		{
		public:
			template<class T> friend class ::residualConnectivity::obs::getSubObservation;
			template<class T> friend class ::residualConnectivity::subObs::getObservation;

			typedef ::residualConnectivity::subObs::articulationConditioningForResampling subObservationType;
			typedef ::residualConnectivity::subObs::withWeightConstructorType subObservationConstructorType;

			articulationConditioningForResampling(context const& contextObj, boost::mt19937& randomSource);
			articulationConditioningForResampling(articulationConditioningForResampling&& other);
			articulationConditioningForResampling& operator=(articulationConditioningForResampling&& other);
			articulationConditioningForResampling(context const& contextObj, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::withWeightConstructorType&);
			const mpfr_class& getWeight() const;
		private:
			void getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const;
			mpfr_class weight;
		};
	}
}
#endif
