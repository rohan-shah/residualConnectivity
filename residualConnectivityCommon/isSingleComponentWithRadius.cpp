#include "isSingleComponentWithRadius.h"
#include <boost/graph/connected_components.hpp>
#include "depth_first_search_restricted.hpp"
#include "connected_components_restricted.hpp"
namespace residualConnectivity
{
	bool isSingleComponentPossible(const context::inputGraph& graph, const vertexState* state, std::vector<int>& connectedComponents, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack)
	{
		std::size_t nVertices = boost::num_vertices(graph);

		std::vector<context::inputGraph::vertex_descriptor> initialPoints;
		typedef boost::color_traits<boost::default_color_type> Color;
		std::vector<boost::default_color_type> colorMap(nVertices, Color::black());

		for(std::size_t i = 0; i < nVertices; i++)
		{
			if(state[i].state & FIXED_ON)
			{
				initialPoints.push_back((int)i);
			}
			if(state[i].state & (ON_MASK | UNFIXED_OFF)) colorMap[i] = Color::white();
		}

		if(connectedComponents.size() < nVertices) connectedComponents.resize(nVertices);
		int nComponents = boost::connected_components_restricted(graph, &(connectedComponents[0]), &(colorMap[0]), stack, initialPoints);
		
		return nComponents <= 1;
	}
	bool isSingleComponentAllOn(context const& contextObj, const vertexState* state, std::vector<int>& connectedComponents, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack)
	{
		const context::inputGraph& graph = contextObj.getGraph();
		std::size_t nVertices = boost::num_vertices(graph);

		std::vector<context::inputGraph::vertex_descriptor> initialPoints;
		typedef boost::color_traits<boost::default_color_type> Color;
		std::vector<boost::default_color_type> colorMap(nVertices, Color::black());

		for(std::size_t i = 0; i < nVertices; i++)
		{
			if(state[i].state & ON_MASK)
			{
				initialPoints.push_back((int)i);
				colorMap[i] = Color::white();
			}
		}
		

		if(connectedComponents.size() < nVertices) connectedComponents.resize(nVertices);
		int nComponents = boost::connected_components_restricted(graph, &(connectedComponents[0]), &(colorMap[0]), stack, initialPoints);
		
		return nComponents <= 1;
	}
	bool isSingleComponentSpecified(context const& contextObj, const std::vector<context::inputGraph::vertex_descriptor>& specifiedVertices, std::vector<int>& connectedComponents, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack)
	{
		const context::inputGraph& graph = contextObj.getGraph();
		std::size_t nVertices = boost::num_vertices(graph);

		typedef boost::color_traits<boost::default_color_type> Color;
		std::vector<boost::default_color_type> colorMap(nVertices, Color::black());

		for(std::vector<context::inputGraph::vertex_descriptor>::const_iterator i = specifiedVertices.begin(); i != specifiedVertices.end(); i++)
		{
			colorMap[*i] = Color::white();
		}
		
		if(connectedComponents.size() < nVertices) connectedComponents.resize(nVertices);
		int nComponents = boost::connected_components_restricted(graph, &(connectedComponents[0]), &(colorMap[0]), stack, specifiedVertices);
		
		return nComponents <= 1;
	}
	bool partIsSingleComponent(context const& contextObj, const vertexState* state, const std::vector<context::inputGraph::vertex_descriptor>& importantPoints, std::vector<context::inputGraph::vertex_descriptor>& otherVertices, std::vector<int>& connectedComponents, boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType& stack)
	{
		const context::inputGraph& graph = contextObj.getGraph();
		std::size_t nVertices = boost::num_vertices(graph);

		typedef boost::color_traits<boost::default_color_type> Color;
		std::vector<boost::default_color_type> colorMap(nVertices, Color::black());

		for(std::vector<context::inputGraph::vertex_descriptor>::const_iterator i = importantPoints.begin(); i != importantPoints.end(); i++)
		{
			if(state[*i].state & ON_MASK)
			{
				colorMap[*i] = Color::white();
			}
		}
		for(std::vector<context::inputGraph::vertex_descriptor>::const_iterator i = otherVertices.begin(); i != otherVertices.end(); i++)
		{
			if(state[*i].state & ON_MASK) colorMap[*i] = Color::white();
		}
		
		if(connectedComponents.size() < nVertices) connectedComponents.resize(nVertices);
		int nComponents = boost::connected_components_restricted(graph, &(connectedComponents[0]), &(colorMap[0]), stack, importantPoints);
		return nComponents <= 1;

	}

}
