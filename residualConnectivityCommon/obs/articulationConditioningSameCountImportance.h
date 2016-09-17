#ifndef RESIDUAL_CONNECTIVITY_ARTICULATION_CONDITIONING_SAME_COUNT_IMPORTANCE_OBS_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_ARTICULATION_CONDITIONING_SAME_COUNT_IMPORTANCE_OBS_HEADER_GUARD
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
		class articulationConditioningSameCountImportance;
	}
	namespace obs
	{
		class articulationConditioningSameCountImportance : public ::residualConnectivity::withSub
		{
		public:
			template<class T> friend class ::residualConnectivity::obs::getSubObservation;
			template<class T> friend class ::residualConnectivity::subObs::getObservation;

			typedef ::residualConnectivity::subObs::articulationConditioningSameCountImportance subObservationType;
			typedef ::residualConnectivity::subObs::articulationConditioningSameCountImportanceConstructorType subObservationConstructorType;

			articulationConditioningSameCountImportance(context const& contextObj, const std::vector<double>* importanceProbabilities, boost::mt19937& randomSource);
			articulationConditioningSameCountImportance(articulationConditioningSameCountImportance&& other);
			articulationConditioningSameCountImportance& operator=(articulationConditioningSameCountImportance&& other);
			articulationConditioningSameCountImportance(context const& contextObj, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::articulationConditioningSameCountImportanceConstructorType&);
			const mpfr_class& getWeight() const;
		private:
			void getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const;
			mpfr_class weight;
			int nUpVertices;
			const std::vector<double>* importanceProbabilities;
		};
	}
}
#endif
