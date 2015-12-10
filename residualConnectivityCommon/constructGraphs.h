#ifndef CONSTRUCT_GRAPHS_HEADER_GUARD
#define CONSTRUCT_GRAPHS_HEADER_GUARD
#include "Context.h"
namespace residualConnectivity
{
	namespace constructGraphs
	{
		void squareTorus(int dimension, boost::shared_ptr<Context::inputGraph>& graph, boost::shared_ptr<std::vector<Context::vertexPosition> >& vertexPositions);
		void squareGrid(int gridDimension, boost::shared_ptr<Context::inputGraph>& graph, boost::shared_ptr<std::vector<Context::vertexPosition> >& vertexPositions);
		void hexagonalTiling(int dimensionX, int dimensionY, boost::shared_ptr<Context::inputGraph>& graph, boost::shared_ptr<std::vector<Context::vertexPosition> >& vertexPositions);
	}
}
#endif