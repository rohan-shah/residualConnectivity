#ifndef RESIDUAL_CONNECTIVITY_USING_BICONNECTED_COMPONENTS_OBS_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_USING_BICONNECTED_COMPONENTS_OBS_HEADER_GUARD
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
		class usingBiconnectedComponents;
	}
	namespace obs
	{
		class usingBiconnectedComponents : public ::residualConnectivity::withSub
		{
		public:
			template<class T> friend class ::residualConnectivity::obs::getSubObservation;
			template<class T> friend class ::residualConnectivity::subObs::getObservation;

			typedef ::residualConnectivity::subObs::usingBiconnectedComponents subObservationType;
			typedef ::residualConnectivity::subObs::withWeightConstructorType subObservationConstructorType;

			usingBiconnectedComponents(Context const& context, boost::mt19937& randomSource);
			usingBiconnectedComponents(usingBiconnectedComponents&& other);
			usingBiconnectedComponents& operator=(usingBiconnectedComponents&& other);
			usingBiconnectedComponents(Context const& context, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::withWeightConstructorType&);
			const mpfr_class& getWeight() const;
		private:
			void getSubObservation(vertexState* newState, int radius, subObservationConstructorType& other) const;
			mpfr_class weight;
		};
	}
}
#endif
