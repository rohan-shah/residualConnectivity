#ifndef DISCRETE_GERM_GRAIN_SUB_OBS_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_SUB_OBS_HEADER_GUARD
#include <vector>
#include <boost/random/mersenne_twister.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <set>
#include <boost/shared_ptr.hpp>
#include "Context.h"
namespace discreteGermGrain
{
	class DiscreteGermGrainObs;
	class DiscreteGermGrainSubObs
	{
	public:
		/*
		 * Construct a sub observation with the given points and model parameters
		 * @param[in] context Global data for the problem
		 * @param[in] state The current state of all the vertices
		 * @param[in] currentRadius The scale that was used to choose the subPoints out from the larger, complete set of points
		 */
		DiscreteGermGrainSubObs(Context const& context, const boost::shared_array<const vertexState> state, int currentRadius);
		/*
		 * Move constructor 
		 * @param[in] other The existing object to swap with this one
		 */
		DiscreteGermGrainSubObs(DiscreteGermGrainSubObs&& other);
		/*
		  * Get out a complete point pattern which is consistent with this subpattern at scale currentRadius. A boolean value is returned saying whether or not the complete pattern is connected at scale newRadius.
		  * 
		  * @param[in] newRadius The radius / distance giving the scale on which we want the new pattern to be connected. 
		  * @param[in] randomSource The random number generator used to create the new point pattern
		  * @param[in] componentIDs We call boost::connected_components in this function, which requires a vector. This is input by the calling code to lower memory allocation within this funtion.
		  * @param[out] successful A boolean output parameter that is true if the new pattern had only one connected component at scale newRadius
		  * @ret The generated point pattern
		  */
		DiscreteGermGrainObs getSingleComponentObservation(int newRadius, boost::mt19937& randomSource, std::vector<int>& componentIDs, bool& successful);
		/*
		 * Get out a complete pattern which is consistent with the current subpattern at scale currentRadius, and connected at scale newRadius. 
		 * @param[out] outputObservation The constructed pattern
		 * @param[in] newRadius The scale at which the output pattern should be connected
		 * @param[in] randomSource The random number generator used to create the new complete pattern
		 * @param[in] componentIDs We call boost::connected_components in this function, which requires a vector. This is input by the calling code to lower memory allocation within this funtion.
		 * @param[in] outputProbability An estimate of the probability that a pattern consistent with the current subpattern will be connected at scale newRadius
		*/
		void getSingleComponentObservation(int newRadius, boost::mt19937& randomSource, std::vector<int>& componentIDs, DiscreteGermGrainObs& outputObservation, float& outputProbability);
		/*
		 * Estimate the probability of getting a connected scale-0 graph consistent with the current scale-1 graph. The number of simulations to be performed in obtaining the estimate is nSimulations. If this is positive then an estimate of 0 is possible. If this is -1, then simulation continues until such a connected pattern is found, and outputProbability will always be positive. 
		 * @param[in] randomSource The random number generator used to create the new complete pattern
		 * @param[in] nSimulations The number of simulations to be performed in estimating the desired probability. Can be -1
		 * @param[out] outputProbability The estimated probability. Can be 0 if nSimulations was -1
		 * @ret Resulting complete pattern. Can be an invalid object if no suitable pattern was found. Check outputProbability before using. 
		 */
		boost::shared_array<const vertexState> estimateRadius1(boost::mt19937& randomSource, int nSimulations, float& outputProbability);
		/*
		 * The graph type used throughout. This is specifically an adjacency_list object, because the adjacency_matrix type is *very* badly suited to a lot of the graph algorithms. 
		 */
		typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_index_t, int> > graph_t;
		/*
		 * Construct a graph that is useful for trying to identify a complete pattern which is connected on scale-0, which is consistent with a scale-1 subpattern which is scale-1 connected. This graph includes every point except those that are marked FIXED_OFF
		 * @param[out] randomVertices The indices of all the random vertices. That is, points that MAY be included in the final pattern, but aren't part of the sub-pattern, so they aren't GUARANTEED to be included
		 * @param[out] subPointVertices The indices of all the sub-points within allPoissiblePoints. These are the points that are fixed and guaranteed to be included in any consistent subpattern
		 * @param[out] graph The constructed graph
		 */
		void constructRadius1Graph(graph_t& graph, std::vector<int>& graphVertices) const;

		const vertexState* getState() const;
		Context const& getContext() const;
	private:
		/*
		 * Extract a random point pattern that is consistent with the current pattern at the current scale 
		 * @param[in] randomSource The random number generator used to create the new point pattern
		 * @param[out] outputState The generated state
		 */
		void getRandomPointPattern(boost::mt19937& randomSource, vertexState* outputState);
		//__declspec(noinline) void constructSubgraph(const std::set<int>& subgraphVertexIndices, const std::vector<Config::gridPoint>& allPoints, graph_t& subgraph);
		DiscreteGermGrainSubObs(DiscreteGermGrainSubObs& other);
		
		Context const& context;
		int currentRadius;
		float openProbability;
		boost::shared_array<const vertexState> state;
	};
}
#endif
