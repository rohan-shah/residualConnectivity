#ifndef DISCRETE_GERM_GRAIN_SUB_OBS_USING_BICONNECTED_COMPONENTS_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_SUB_OBS_USING_BICONNECTED_COMPONENTS_HEADER_GUARD
#include "subObs/subObs.h"
#include "Context.h"
#include "subObsTypes.h"
#include "subObs/getObservation.hpp"
#include "obs/getSubObservation.hpp"
namespace discreteGermGrain
{
	namespace obs
	{
		class usingBiconnectedComponents;
	}
	namespace subObs
	{
		class usingBiconnectedComponents : public ::discreteGermGrain::subObs::subObsWithRadius
		{
		public:
			template<class T> friend class ::discreteGermGrain::subObs::getObservation;
			template<class T> friend class ::discreteGermGrain::obs::getSubObservation;

			typedef ::discreteGermGrain::obs::usingBiconnectedComponents observationType;
			typedef ::discreteGermGrain::obs::usingBiconnectedComponentsConstructorType observationConstructorType;

			usingBiconnectedComponents(usingBiconnectedComponents&& other);
			typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_index_t, int> > radiusOneGraphType;
			void constructRadius1Graph(radiusOneGraphType& graph, std::vector<int>& graphVertices) const;
			boost::shared_array<const vertexState> estimateRadius1(boost::mt19937& randomSource, int nSimulations, mpfr_class& outputProbability) const;
			bool isPotentiallyConnected() const;
			usingBiconnectedComponents(Context const& context, boost::shared_array<const vertexState> state, int radius, ::discreteGermGrain::subObs::usingBiconnectedComponentsConstructorType&);
		private:
			void getObservation(vertexState* state, boost::mt19937& randomSource, observationConstructorType&) const;
			bool potentiallyConnected;
		};
	}
}
#endif
