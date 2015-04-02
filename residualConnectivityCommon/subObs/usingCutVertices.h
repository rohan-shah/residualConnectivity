#ifndef DISCRETE_GERM_GRAIN_SUB_OBS_USING_CUT_VERTICES_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_SUB_OBS_USING_CUT_VERTICES_HEADER_GUARD
#include "subObs/subObs.h"
#include "Context.h"
#include "subObsTypes.h"
#include "subObs/getObservation.hpp"
#include "obs/getSubObservation.hpp"
namespace discreteGermGrain
{
	namespace obs
	{
		class usingCutVertices;
	}
	namespace subObs
	{
		class usingCutVertices : public ::discreteGermGrain::subObs::subObsWithRadius
		{
		public:
			template<class T> friend class ::discreteGermGrain::subObs::getObservation;
			template<class T> friend class ::discreteGermGrain::obs::getSubObservation;

			typedef ::discreteGermGrain::obs::usingCutVertices observationType;
			typedef ::discreteGermGrain::obs::usingCutVerticesConstructorType observationConstructorType;

			usingCutVertices(usingCutVertices&& other);
			typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_index_t, int> > radiusOneGraphType;
			void constructRadius1Graph(radiusOneGraphType& graph, std::vector<int>& graphVertices) const;
			boost::shared_array<const vertexState> estimateRadius1(boost::mt19937& randomSource, int nSimulations, std::vector<int>& scratchMemory, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack, mpfr_class& outputProbability) const;
			bool isPotentiallyConnected() const;
			usingCutVertices(Context const& context, boost::shared_array<const vertexState> state, int radius, ::discreteGermGrain::subObs::usingCutVerticesConstructorType&);
		private:
			void getObservation(vertexState* state, boost::mt19937& randomSource, observationConstructorType&) const;
			bool potentiallyConnected;
		};
	}
}
#endif
