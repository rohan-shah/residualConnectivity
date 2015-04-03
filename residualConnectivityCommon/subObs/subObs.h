#ifndef DISCRETE_GERM_GRAIN_SUB_OBS_SUB_OBS_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_SUB_OBS_SUB_OBS_HEADER_GUARD
#include <boost/shared_array.hpp>
#include "Context.h"
#include "observation.h"
#include "subObsTypes.h"
#include "subObs/getObservation.hpp"
namespace discreteGermGrain
{
	namespace subObs
	{
		class subObs : public ::discreteGermGrain::observation
		{
		public:
			template<class T> friend class ::discreteGermGrain::subObs::getObservation;
			typedef ::discreteGermGrain::observation observationType;
			typedef ::discreteGermGrain::obs::basicConstructorType observationConstructorType;

			subObs& operator=(subObs&& other);
			subObs(const Context& context, boost::shared_array<const vertexState> state);
			subObs(subObs&& other);
		private:
			void getObservation(vertexState* state, boost::mt19937& randomSource, observationConstructorType&) const;
		};
		class subObsWithRadius : public subObs
		{
		public:
			subObsWithRadius& operator=(subObsWithRadius&& other);
		protected:
			subObsWithRadius(const Context& context, boost::shared_array<const vertexState> state, int radius);
			subObsWithRadius(subObsWithRadius&& other);
			int radius;
		};
	}
}
#endif
