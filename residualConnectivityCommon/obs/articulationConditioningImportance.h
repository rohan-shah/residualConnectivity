#ifndef RESIDUAL_CONNECTIVITY_ARTICULATION_CONDITIONING_IMPORTANCE_OBS_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_ARTICULATION_CONDITIONING_IMPORTANCE_OBS_HEADER_GUARD
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
		class articulationConditioningImportance;
	}
	namespace obs
	{
		class articulationConditioningImportance : public ::residualConnectivity::withSub
		{
		public:
			template<class T> friend class ::residualConnectivity::obs::getSubObservation;
			template<class T> friend class ::residualConnectivity::subObs::getObservation;

			typedef ::residualConnectivity::subObs::articulationConditioningImportance subObservationType;
			typedef ::residualConnectivity::subObs::withWeightImportanceConstructorType subObservationConstructorType;

			articulationConditioningImportance(context const& contextObj, boost::mt19937& randomSource, const std::vector<double>* importanceProbabilities);
			articulationConditioningImportance(articulationConditioningImportance&& other);
			articulationConditioningImportance& operator=(articulationConditioningImportance&& other);
			articulationConditioningImportance(context const& contextObj, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::withWeightImportanceConstructorType&);
			const mpfr_class& getWeight() const;
		private:
			void getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const;
			mpfr_class weight;
			const std::vector<double>* importanceProbabilities;
		};
	}
}
#endif
