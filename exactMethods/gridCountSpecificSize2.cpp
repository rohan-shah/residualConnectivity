#include "context.h"
#include <boost/scoped_array.hpp>
#include "isSingleComponentWithRadius.h"
#include <omp.h>
#include <boost/graph/connected_components.hpp>
#include "depth_first_search_restricted.hpp"
#include "connected_components_restricted.hpp"
#include "constructGraphs.h"
#ifdef _MSC_VER
	#include <intrin.h>
#endif
namespace residualConnectivity
{
	mpz_class gridCountSpecificSize2(int gridDimension, int size)
	{
		context::inputGraph graph;
		std::vector<context::vertexPosition> vertexPositions;
		constructGraphs::squareGrid(gridDimension, graph, vertexPositions);

		std::size_t connected = 0;
		std::vector<int> connectedComponents;
		connectedComponents.resize(gridDimension*gridDimension);
		boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType stack;

		typedef boost::color_traits<boost::default_color_type> Color;
		std::vector<boost::default_color_type> colors;
		colors.resize(gridDimension*gridDimension);
		std::vector<context::inputGraph::vertex_descriptor> initialPoints;
		initialPoints.reserve(gridDimension*gridDimension);

		//first permunation
		std::size_t currentPermutation = (1ULL << size) - 1;
		std::size_t lastPermutation = ((1ULL << (gridDimension*gridDimension)) - 1) - ((1ULL << (gridDimension*gridDimension-size)) - 1);
		do
		{
			initialPoints.clear();
			for(int i = 0; i < gridDimension*gridDimension; i++)
			{
				if(currentPermutation & (1ULL << i))
				{
					colors[i] = Color::white();
					initialPoints.push_back(i);
				}
				else colors[i] = Color::black();
			}
			int nComponents = boost::connected_components_restricted(graph, &(connectedComponents[0]), &(colors[0]), stack, initialPoints);
			if(nComponents <= 1)
			{
				connected++;
			}
			if(currentPermutation == lastPermutation) break;
			std::size_t t = currentPermutation | (currentPermutation-1);
#ifdef _MSC_VER
			unsigned long index;
			_BitScanForward64(&index, currentPermutation);
			currentPermutation = (t + 1) | (((~t & -~t) - 1) >> (index + 1));
#else
			currentPermutation =(t + 1) | (((~t & -~t) - 1) >> (__builtin_ctzll(currentPermutation) + 1));
#endif
		}
		while(true);
		return connected;
	}
}
