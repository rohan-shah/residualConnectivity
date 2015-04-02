#ifndef DISCRETE_GERM_GRAIN_SUB_OBS_BASIC_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_SUB_OBS_BASIC_HEADER_GUARD
#include "subObs/subObs.h"
#include "Context.h"
#include "subObsTypes.h"
#include "subObs/getObservation.hpp"
#include "obs/getSubObservation.hpp"
namespace discreteGermGrain
{
	namespace obs
	{
		class basic;
	}
	namespace subObs
	{
		class basic : public ::discreteGermGrain::subObs::subObsWithRadius
		{
		public:
			template<class T> friend class ::discreteGermGrain::subObs::getObservation;
			template<class T> friend class ::discreteGermGrain::obs::getSubObservation;

			typedef ::discreteGermGrain::obs::basic observationType;
			typedef ::discreteGermGrain::obs::basicConstructorType observationConstructorType;

			basic(basic&& other);
			bool isPotentiallyConnected() const;
			basic(Context const& context, boost::shared_array<const vertexState> state, int radius, ::discreteGermGrain::subObs::basicConstructorType&);
		private:
			void getObservation(vertexState* state, boost::mt19937& randomSource, observationConstructorType&) const;
			bool potentiallyConnected;
		};
	}
}
#endif
