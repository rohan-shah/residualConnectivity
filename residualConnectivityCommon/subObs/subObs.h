#ifndef RESIDUAL_CONNECTIVITY_SUB_OBS_SUB_OBS_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_SUB_OBS_SUB_OBS_HEADER_GUARD
#include <boost/shared_array.hpp>
#include "context.h"
#include "observation.h"
#include "subObsTypes.h"
#include "subObs/getObservation.hpp"
namespace residualConnectivity
{
	namespace subObs
	{
		class subObs : public ::residualConnectivity::observation
		{
		public:
			template<class T> friend class ::residualConnectivity::subObs::getObservation;
			typedef ::residualConnectivity::observation observationType;
			typedef ::residualConnectivity::obs::basicConstructorType observationConstructorType;

			subObs& operator=(subObs&& other);
			subObs(const context& contextObj, boost::shared_array<const vertexState> state);
			subObs(subObs&& other);
		protected:
			subObs(const subObs& other);
			subObs& operator=(const subObs& other);
		private:
			void getObservation(vertexState* state, boost::mt19937& randomSource, observationConstructorType&) const;
		};
		class subObsWithRadius : public subObs
		{
		public:
			subObsWithRadius& operator=(subObsWithRadius&& other);
			int getRadius() const
			{
				return radius;
			}
		protected:
			subObsWithRadius(const context& contextObj, boost::shared_array<const vertexState> state, int radius);
			subObsWithRadius& operator=(const subObsWithRadius&);
			subObsWithRadius(subObsWithRadius&& other);
			subObsWithRadius(const subObsWithRadius& other);
			int radius;
		};
	}
}
#endif
