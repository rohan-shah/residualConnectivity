#ifndef RESIDUAL_CONNECTIVITY_SUB_OBS_ARTICULATION_CONDITIONING_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_SUB_OBS_ARTICULATION_CONDITIONING_HEADER_GUARD
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
		class articulationConditioning;
	}
	namespace subObs
	{
		class articulationConditioning : public ::residualConnectivity::subObs::withWeight
		{
		public:
			template<class T> friend class ::residualConnectivity::subObs::getObservation;
			template<class T> friend class ::residualConnectivity::obs::getSubObservation;

			typedef ::residualConnectivity::obs::articulationConditioning observationType;
			typedef ::residualConnectivity::obs::withWeightConstructorType observationConstructorType;

			articulationConditioning(articulationConditioning&& other);
			bool isPotentiallyConnected() const;
			articulationConditioning(context const& contextObj, boost::shared_array<vertexState> state, int radius, ::residualConnectivity::subObs::withWeightConstructorType &);
			void getObservation(vertexState* state, boost::mt19937& randomSource, observationConstructorType&) const;
			articulationConditioning copyWithWeight(mpfr_class weight) const;
			mpfr_class estimateRadius1(boost::mt19937& randomSource, int nSimulations, std::vector<int>& scratchMemory, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack) const;
		protected:
			articulationConditioning(const articulationConditioning& other, mpfr_class weight);
		private:
			articulationConditioning(const articulationConditioning& other);
			bool potentiallyConnected;
		};
	}
}
#endif
