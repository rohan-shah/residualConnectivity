#ifndef RESIDUAL_CONNECTIVITY_BASIC_OBS_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_BASIC_OBS_HEADER_GUARD
#include "subObsTypes.h"
#include "obs/getSubObservation.hpp"
#include "subObs/getObservation.hpp"
#include "obs/withSub.h"
#include <boost/shared_array.hpp>
#include "Context.h"
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

			basic(Context const& context, boost::mt19937& randomSource);
			basic(Context const& context, boost::shared_array<const vertexState> state);
			basic(basic&& other);
			basic& operator=(basic&& other);
		private:
			void getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const;
			basic(Context const& context, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::basicConstructorType&);
		};
	}
}
#endif
