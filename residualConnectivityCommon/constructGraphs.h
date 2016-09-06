#ifndef CONSTRUCT_GRAPHS_HEADER_GUARD
#define CONSTRUCT_GRAPHS_HEADER_GUARD
#include "context.h"
namespace residualConnectivity
{
	namespace constructGraphs
	{
		void squareTorus(int dimension, context::inputGraph& graph, std::vector<context::vertexPosition>& vertexPositions);
		void squareGrid(int gridDimension, context::inputGraph& graph, std::vector<context::vertexPosition>& vertexPositions);
		void hexagonalTiling(int dimensionX, int dimensionY, context::inputGraph& graph, std::vector<context::vertexPosition>& vertexPositions);
	}
}
#endif