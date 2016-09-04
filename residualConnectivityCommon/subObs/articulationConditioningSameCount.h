#ifndef RESIDUAL_CONNECTIVITY_SUB_OBS_ARTICULATION_CONDITIONING_SAME_DIST_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_SUB_OBS_ARTICULATION_CONDITIONING_SAME_DIST_HEADER_GUARD
#include "subObs/subObs.h"
#include "context.h"
#include "subObsTypes.h"
#include "subObs/getObservation.hpp"
#include "obs/getSubObservation.hpp"
#include "subObs/withWeight.h"
#include "depth_first_search_restricted.hpp"
namespace residualConnectivity
{
	namespace obs
	{
		class articulationConditioningSameCount;
	}
	namespace subObs
	{
		class articulationConditioningSameCount : public ::residualConnectivity::subObs::withWeight
		{
		public:
			template<class T> friend class ::residualConnectivity::subObs::getObservation;
			template<class T> friend class ::residualConnectivity::obs::getSubObservation;

			typedef ::residualConnectivity::obs::articulationConditioningSameCount observationType;
			typedef ::residualConnectivity::obs::weightAndCountConstructorType observationConstructorType;

			articulationConditioningSameCount(articulationConditioningSameCount&& other);
			bool isPotentiallyConnected() const;
			articulationConditioningSameCount(context const& contextObj, boost::shared_array<vertexState> state, int radius, ::residualConnectivity::subObs::articulationConditioningSameCountConstructorType &);
			void getObservation(vertexState* state, boost::mt19937& randomSource, observationConstructorType&) const;
			articulationConditioningSameCount copyWithWeight(mpfr_class weight) const;
		protected:
			articulationConditioningSameCount(const articulationConditioningSameCount& other, mpfr_class weight, int nUpVertices);
		private:
			void conditionArticulation(boost::shared_array<vertexState> state, std::vector<int>& scratch, boost::detail::depth_first_visit_restricted_impl_helper<subGraphType>::stackType& subGraphStack, subGraphType& subGraph);
			articulationConditioningSameCount(const articulationConditioningSameCount& other);
			bool potentiallyConnected;
			int nUpVertices;
		};
	}
}
#endif
