#ifndef RESIDUAL_CONNECTIVITY_ARTICULATION_CONDITIONING_SAME_COUNT_OBS_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_ARTICULATION_CONDITIONING_SAME_COUNT_OBS_HEADER_GUARD
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
		class articulationConditioningSameCount;
	}
	namespace obs
	{
		class articulationConditioningSameCount : public ::residualConnectivity::withSub
		{
		public:
			template<class T> friend class ::residualConnectivity::obs::getSubObservation;
			template<class T> friend class ::residualConnectivity::subObs::getObservation;

			typedef ::residualConnectivity::subObs::articulationConditioningSameCount subObservationType;
			typedef ::residualConnectivity::subObs::articulationConditioningSameCountConstructorType subObservationConstructorType;

			articulationConditioningSameCount(context const& contextObj, boost::mt19937& randomSource);
			articulationConditioningSameCount(articulationConditioningSameCount&& other);
			articulationConditioningSameCount& operator=(articulationConditioningSameCount&& other);
			articulationConditioningSameCount(context const& contextObj, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::weightAndCountConstructorType&);
			const mpfr_class& getWeight() const;
		private:
			void getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const;
			mpfr_class weight;
			int nUpVertices;
		};
	}
}
#endif
