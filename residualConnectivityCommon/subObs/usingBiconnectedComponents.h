#ifndef DISCRETE_GERM_GRAIN_SUB_OBS_USING_BICONNECTED_COMPONENTS_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_SUB_OBS_USING_BICONNECTED_COMPONENTS_HEADER_GUARD
#include "subObs/subObs.h"
#include "Context.h"
#include "subObsTypes.h"
#include "subObs/getObservation.hpp"
#include "obs/getSubObservation.hpp"
#include "subObs/withWeight.h"
namespace discreteGermGrain
{
	namespace obs
	{
		class usingBiconnectedComponents;
	}
	namespace subObs
	{
		class usingBiconnectedComponents : public ::discreteGermGrain::subObs::withWeight
		{
		public:
			template<class T> friend class ::discreteGermGrain::subObs::getObservation;
			template<class T> friend class ::discreteGermGrain::obs::getSubObservation;

			typedef ::discreteGermGrain::obs::usingBiconnectedComponents observationType;
			typedef ::discreteGermGrain::obs::withWeightConstructorType observationConstructorType;

			usingBiconnectedComponents(usingBiconnectedComponents&& other);
			void estimateRadius1(boost::mt19937& randomSource, int nSimulations, std::vector<int>& scratchMemory, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack, std::vector<observationType>& outputObservations) const;
			bool isPotentiallyConnected() const;
			usingBiconnectedComponents(Context const& context, boost::shared_array<const vertexState> state, int radius, ::discreteGermGrain::subObs::withWeightConstructorType &);
			void getObservation(vertexState* state, boost::mt19937& randomSource, observationConstructorType&) const;
		private:
			bool potentiallyConnected;
		};
	}
}
#endif
