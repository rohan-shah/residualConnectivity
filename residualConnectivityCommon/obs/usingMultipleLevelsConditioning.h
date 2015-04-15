#ifndef DISCRETE_GERM_GRAIN_USING_MULTIPLE_LEVELS_OBS_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_USING_MULTIPLE_LEVELS_OBS_HEADER_GUARD
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
		class usingMultipleLevelsConditioning;
	}
	namespace obs
	{
		class usingMultipleLevelsConditioning : public ::discreteGermGrain::withSub
		{
		public:
			template<class T> friend class ::discreteGermGrain::obs::getSubObservation;
			template<class T> friend class ::discreteGermGrain::subObs::getObservation;

			typedef ::discreteGermGrain::subObs::usingMultipleLevelsConditioning subObservationType;
			typedef ::discreteGermGrain::subObs::withWeightConstructorType subObservationConstructorType;

			usingMultipleLevelsConditioning(Context const& context, boost::mt19937& randomSource);
			usingMultipleLevelsConditioning(usingMultipleLevelsConditioning&& other);
			usingMultipleLevelsConditioning& operator=(usingMultipleLevelsConditioning&& other);
			usingMultipleLevelsConditioning(Context const& context, boost::shared_array<const vertexState> state, ::discreteGermGrain::obs::withWeightConstructorType&);
			const mpfr_class& getWeight() const;
		private:
			void getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const;
			mpfr_class weight;
		};
	}
}
#endif
