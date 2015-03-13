#include "isSingleComponentWithRadius.h"
#include <boost/graph/connected_components.hpp>
#include "depth_first_search_restricted.hpp"
#include "connected_components_restricted.hpp"
namespace discreteGermGrain
{
	bool isSingleComponentPossible(Context const& context, const vertexState* state, std::vector<int>& connectedComponents, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack)
	{
		std::size_t nVertices = context.nVertices();

		std::vector<Context::inputGraph::vertex_descriptor> initialPoints;
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
		int nComponents = boost::connected_components_restricted(context.getGraph(), &(connectedComponents[0]), &(colorMap[0]), stack, initialPoints);
		
		return nComponents <= 1;
	}
	bool isSingleComponentAllOn(Context const& context, const vertexState* state, std::vector<int>& connectedComponents, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack)
	{
		std::size_t nVertices = context.nVertices();

		std::vector<Context::inputGraph::vertex_descriptor> initialPoints;
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
		int nComponents = boost::connected_components_restricted(context.getGraph(), &(connectedComponents[0]), &(colorMap[0]), stack, initialPoints);
		
		return nComponents <= 1;
	}
	bool isSingleComponentSpecified(Context const& context, const std::vector<Context::inputGraph::vertex_descriptor>& specifiedVertices, std::vector<int>& connectedComponents, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack)
	{
		std::size_t nVertices = context.nVertices();

		typedef boost::color_traits<boost::default_color_type> Color;
		std::vector<boost::default_color_type> colorMap(nVertices, Color::black());

		for(std::vector<Context::inputGraph::vertex_descriptor>::const_iterator i = specifiedVertices.begin(); i != specifiedVertices.end(); i++)
		{
			colorMap[*i] = Color::white();
		}
		
		if(connectedComponents.size() < nVertices) connectedComponents.resize(nVertices);
		int nComponents = boost::connected_components_restricted(context.getGraph(), &(connectedComponents[0]), &(colorMap[0]), stack, specifiedVertices);
		
		return nComponents <= 1;
	}
}
