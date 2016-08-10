#ifndef RESIDUAL_CONNECTIVITY_SUB_OBS_USING_CUT_VERTICES_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_SUB_OBS_USING_CUT_VERTICES_HEADER_GUARD
#include "subObs/subObs.h"
#include "context.h"
#include "subObsTypes.h"
#include "subObs/getObservation.hpp"
#include "obs/getSubObservation.hpp"
namespace residualConnectivity
{
	namespace obs
	{
		class usingCutVertices;
	}
	namespace subObs
	{
		class usingCutVertices : public ::residualConnectivity::subObs::subObsWithRadius
		{
		public:
			template<class T> friend class ::residualConnectivity::subObs::getObservation;
			template<class T> friend class ::residualConnectivity::obs::getSubObservation;

			typedef ::residualConnectivity::obs::usingCutVertices observationType;
			typedef ::residualConnectivity::obs::withWeightConstructorType observationConstructorType;

			usingCutVertices(usingCutVertices&& other);
			typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_index_t, int> > radiusOneGraphType;
			void constructRadius1Graph(radiusOneGraphType& graph, std::vector<int>& graphVertices) const;
			void estimateRadius1(boost::mt19937& randomSource, int nSimulations, std::vector<int>& scratchMemory, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack, std::vector<observationType>& outputObservations) const;
			bool isPotentiallyConnected() const;
			usingCutVertices(context const& contextObj, boost::shared_array<const vertexState> state, int radius, ::residualConnectivity::subObs::withWeightConstructorType&);
		private:
			void getObservation(vertexState* state, boost::mt19937& randomSource, observationConstructorType&) const;
			bool potentiallyConnected;
		};
	}
}
#endif
