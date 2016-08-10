#ifndef RESIDUAL_CONNECTIVITY_SUB_OBS_USING_BICONNECTED_COMPONENTS_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_SUB_OBS_USING_BICONNECTED_COMPONENTS_HEADER_GUARD
#include "subObs/subObs.h"
#include "context.h"
#include "subObsTypes.h"
#include "subObs/getObservation.hpp"
#include "obs/getSubObservation.hpp"
#include "subObs/withWeight.h"
namespace residualConnectivity
{
	namespace obs
	{
		class usingBiconnectedComponents;
	}
	namespace subObs
	{
		class usingBiconnectedComponents : public ::residualConnectivity::subObs::withWeight
		{
		public:
			template<class T> friend class ::residualConnectivity::subObs::getObservation;
			template<class T> friend class ::residualConnectivity::obs::getSubObservation;

			typedef ::residualConnectivity::obs::usingBiconnectedComponents observationType;
			typedef ::residualConnectivity::obs::withWeightConstructorType observationConstructorType;

			usingBiconnectedComponents(usingBiconnectedComponents&& other);
			void estimateRadius1(boost::mt19937& randomSource, int nSimulations, std::vector<int>& scratchMemory, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack, std::vector<observationType>& outputObservations) const;
			bool isPotentiallyConnected() const;
			usingBiconnectedComponents(context const& contextObj, boost::shared_array<const vertexState> state, int radius, ::residualConnectivity::subObs::withWeightConstructorType &);
			void getObservation(vertexState* state, boost::mt19937& randomSource, observationConstructorType&) const;
		private:
			bool potentiallyConnected;
		};
	}
}
#endif
