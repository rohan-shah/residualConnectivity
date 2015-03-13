#ifndef DISCRETE_GERM_GRAIN_BASIC_OBS_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_BASIC_OBS_HEADER_GUARD
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
		class basic;
	}
	namespace obs
	{
		class basic : public ::discreteGermGrain::withSub
		{
		public:
			template<class T> friend class ::discreteGermGrain::obs::getSubObservation;
			template<class T> friend class ::discreteGermGrain::subObs::getObservation;

			typedef ::discreteGermGrain::subObs::basic subObservationType;
			typedef ::discreteGermGrain::subObs::basicConstructorType subObservationConstructorType;

			basic(Context const& context, boost::mt19937& randomSource);
			basic(Context const& context, boost::shared_array<const vertexState> state);
			basic(basic&& other);
			basic& operator=(basic&& other);
		private:
			void getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const;
			basic(Context const& context, boost::shared_array<const vertexState> state, ::discreteGermGrain::obs::basicConstructorType&);
		};
	}
}
#endif
