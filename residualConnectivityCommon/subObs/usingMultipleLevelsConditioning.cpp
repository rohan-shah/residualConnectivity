#include "subObs/usingMultipleLevelsConditioning.h"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/graph/biconnected_components.hpp>
#include "isSingleComponentWithRadius.h"
#include "subObs/usingMultipleLevelsConditioning.h"
#include "obs/usingMultipleLevelsConditioning.h"
#include "constructSubGraph.h"
#include "subObs/withFinalConditioning.hpp"
namespace discreteGermGrain
{
	namespace subObs
	{
		struct isFixedOnTerminator
		{
		public:
			isFixedOnTerminator(const vertexState* state, std::vector<int>& graphVertices)
				:found(false), state(state), graphVertices(graphVertices)
			{}
			bool operator()(subGraphType::vertex_descriptor& u, const subGraphType& graph)
			{
				return (found = (state[graphVertices[u]].state == FIXED_ON));
			}
			bool found;
			const vertexState* state;
			std::vector<int>& graphVertices;
		};
		usingMultipleLevelsConditioning::usingMultipleLevelsConditioning(Context const& context, boost::shared_array<vertexState> state, int radius, ::discreteGermGrain::subObs::withWeightConstructorType& otherData)
			: ::discreteGermGrain::subObs::withWeight(context, state, radius, otherData.weight)
		{
			potentiallyConnected = isSingleComponentPossible(context, state.get(), otherData.components, otherData.stack);
			//construct subgraph
			subGraphType graph;
			//Reuse components vector here
			std::vector<int>& graphVertices = otherData.components;
			{
				constructSubGraph(graph, graphVertices, context, state.get());
				//assign edge indices
				subGraphType::edge_iterator start, end;
				int edgeIndex = 0;
				boost::property_map<subGraphType, boost::edge_index_t>::type edgePropertyMap = boost::get(boost::edge_index, graph);
				boost::tie(start, end) = boost::edges(graph);
				for(; start != end; start++) boost::put(edgePropertyMap, *start, edgeIndex++);
			}
			//get out biconnected components of helper graph (which has different vertex ids, remember)
			std::vector<int> articulationVertices;
			boost::articulation_points(graph, std::back_inserter(articulationVertices));
			int nImportanceVertices = 0;
		
			typedef boost::color_traits<boost::default_color_type> Color;
			std::vector<boost::default_color_type> colorMap(boost::num_vertices(graph), Color::white());
			isFixedOnTerminator terminator(getState(), graphVertices);

			for(std::vector<int>::iterator i = articulationVertices.begin(); i != articulationVertices.end(); i++)
			{
				std::fill(colorMap.begin(), colorMap.end(), Color::white());
				colorMap[*i] = Color::black();

				subGraphType::out_edge_iterator current, end;
				boost::tie(current, end) = boost::out_edges(*i, graph);
				int nComponentsWithFixedOnVertices = 0;
				terminator.found = false;
				for(; current != end; current++)
				{
					int otherVertex = current->m_target;
					auto nullVisitor = boost::make_dfs_visitor(boost::null_visitor());
					boost::detail::depth_first_visit_restricted_impl(graph, otherVertex, nullVisitor, &(colorMap[0]), otherData.subGraphStack, boost::ref(terminator));
					if(terminator.found) nComponentsWithFixedOnVertices++;
					if(nComponentsWithFixedOnVertices > 1) break;
				}
				if(nComponentsWithFixedOnVertices > 1)
				{
					nImportanceVertices++;
					state[graphVertices[*i]].state = FIXED_ON;
				}
			}
			weight *= boost::multiprecision::pow(context.getOperationalProbability(), nImportanceVertices);
		}
		bool usingMultipleLevelsConditioning::isPotentiallyConnected() const
		{
			return potentiallyConnected;
		}
		usingMultipleLevelsConditioning::usingMultipleLevelsConditioning(usingMultipleLevelsConditioning&& other)
			: ::discreteGermGrain::subObs::withWeight(static_cast<::discreteGermGrain::subObs::withWeight&&>(other)), potentiallyConnected(other.potentiallyConnected)
		{
			potentiallyConnected = other.potentiallyConnected;
		}
		void usingMultipleLevelsConditioning::getObservation(vertexState* outputState, boost::mt19937& randomSource, observationConstructorType& otherData)const
		{
			boost::random::bernoulli_distribution<float> vertexDistribution(context.getOperationalProbabilityD());
			std::size_t nVertices = context.nVertices();
			memcpy(outputState, state.get(), sizeof(vertexState)*nVertices);
			//generate a full random grid, which includes the subPoints 
			for(std::size_t i = 0; i < nVertices; i++)
			{
				if(outputState[i].state & UNFIXED_MASK)
				{
					if(vertexDistribution(randomSource))
					{
						outputState[i].state = UNFIXED_ON;
					}
					else outputState[i].state = UNFIXED_OFF;
				}
			}
			otherData.weight = weight;
		}
		void usingMultipleLevelsConditioning::estimateRadius1(boost::mt19937& randomSource, int nSimulations, std::vector<int>& scratchMemory, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack, std::vector<observationType>& outputObservations) const
		{
			if(radius != 1)
			{
				throw std::runtime_error("Radius must be 1 to call constructRadius1Graph");
			}
			withFinalConditioning::estimateRadius1<usingMultipleLevelsConditioning>(*this, randomSource, nSimulations, scratchMemory, stack, outputObservations);
		}
	}
}
