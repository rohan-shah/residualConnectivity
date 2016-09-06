#ifndef RESIDUAL_CONNECTIVITY_BASIC_OBS_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_BASIC_OBS_HEADER_GUARD
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
		class basic;
	}
	namespace obs
	{
		class basic : public ::residualConnectivity::withSub
		{
		public:
			template<class T> friend class ::residualConnectivity::obs::getSubObservation;
			template<class T> friend class ::residualConnectivity::subObs::getObservation;

			typedef ::residualConnectivity::subObs::basic subObservationType;
			typedef ::residualConnectivity::subObs::basicConstructorType subObservationConstructorType;

			basic(context const& contextObj, boost::mt19937& randomSource);
			basic(context const& contextObj, boost::shared_array<const vertexState> state);
			basic(basic&& other);
			basic& operator=(basic&& other);
		private:
			void getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const;
			basic(context const& contextObj, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::basicConstructorType&);
		};
	}
}
#endif
