#ifndef RESIDUAL_CONNECTIVITY_SUB_OBS_BASIC_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_SUB_OBS_BASIC_HEADER_GUARD
#include "subObs/subObs.h"
#include "Context.h"
#include "subObsTypes.h"
#include "subObs/getObservation.hpp"
#include "obs/getSubObservation.hpp"
namespace residualConnectivity
{
	namespace obs
	{
		class basic;
	}
	namespace subObs
	{
		class basic : public ::residualConnectivity::subObs::subObsWithRadius
		{
		public:
			template<class T> friend class ::residualConnectivity::subObs::getObservation;
			template<class T> friend class ::residualConnectivity::obs::getSubObservation;

			typedef ::residualConnectivity::obs::basic observationType;
			typedef ::residualConnectivity::obs::basicConstructorType observationConstructorType;

			basic(basic&& other);
			bool isPotentiallyConnected() const;
			basic(Context const& context, boost::shared_array<const vertexState> state, int radius, ::residualConnectivity::subObs::basicConstructorType&);
		private:
			void getObservation(vertexState* state, boost::mt19937& randomSource, observationConstructorType&) const;
			bool potentiallyConnected;
		};
	}
}
#endif
