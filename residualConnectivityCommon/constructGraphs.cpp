#include "constructGraphs.h"
#define M_PI 3.14159265358979323846
namespace discreteGermGrain
{
	namespace constructGraphs
	{
		void squareTorus(int dimension, boost::shared_ptr<Context::inputGraph>& graph, boost::shared_ptr<std::vector<Context::vertexPosition> >& vertexPositions)
		{
			graph.reset(new Context::inputGraph(dimension * dimension));
			vertexPositions.reset(new std::vector<Context::vertexPosition>(dimension * dimension));
			for(int i = 0; i < dimension; i++)
			{
				for(int j = 0; j < dimension; j++)
				{
					(*vertexPositions)[i + j * dimension] = Context::vertexPosition((float)i, (float)j);

					if(i != dimension - 1) 
					{
						boost::add_edge(i + j*dimension, i + 1 +j*dimension, *graph);
					}
					else
					{
						boost::add_edge(i + j*dimension, 0 +j*dimension, *graph);
					}
					if(j != dimension - 1) 
					{
						boost::add_edge(i + j*dimension, i + (j+1)*dimension, *graph);
					}
					else
					{
						boost::add_edge(i + j*dimension, i + 0*dimension, *graph);
					}
				}
			}
		}
		void squareGrid(int gridDimension, boost::shared_ptr<Context::inputGraph>& graph, boost::shared_ptr<std::vector<Context::vertexPosition> >& vertexPositions)
		{
			graph.reset(new Context::inputGraph(gridDimension * gridDimension));
			vertexPositions.reset(new std::vector<Context::vertexPosition>(gridDimension * gridDimension));
			for(int i = 0; i < gridDimension; i++)
			{
				for(int j = 0; j < gridDimension; j++)
				{
					(*vertexPositions)[i + j * gridDimension] = Context::vertexPosition((float)i*100, (float)j*100);

					if(i != gridDimension - 1) boost::add_edge(i + j*gridDimension, i + 1 +j*gridDimension, *graph);
					if(j != gridDimension - 1) boost::add_edge(i + j*gridDimension, i + (j+1)*gridDimension, *graph);
				}
			}
		}
		void hexagonalTiling(int dimensionX, int dimensionY, boost::shared_ptr<Context::inputGraph>& graph, boost::shared_ptr<std::vector<Context::vertexPosition> >& vertexPositions)
		{
			int nVertices = 6 + (4 * (dimensionY-1)) + (4 + 2 * (dimensionY-1)) * (dimensionX- 1);
			graph.reset(new Context::inputGraph(nVertices));
			vertexPositions.reset(new std::vector<Context::vertexPosition>(nVertices));
			for (int i = 0; i < dimensionY * 2; i++)
			{
				//Connect up the left side of the first column
				boost::add_edge(i, i + 1, *graph);
				//Connect up the right side of the first column
				boost::add_edge(i+dimensionY*2+1, i + 2 + dimensionY*2, *graph);
			}
			//Make horizontal connections
			for (int i = 0; i <= dimensionY; i++)
			{
				boost::add_edge(2 * i, 2 * i + dimensionY * 2 + 1, *graph);
			}
			//Add one last vertical edge for the next column (if there is one)
			if (dimensionX > 1)
			{
				boost::add_edge(6 + 4 * (dimensionY - 1)-1, 6 + 4 * (dimensionY - 1), *graph);
			}
			for (int i = 1; i < dimensionX; i++)
			{
				//Number of total vertices we would need to finish i columns with the specified gridDimensionY
				int toFinish = 6 + (4 * (dimensionY-1)) + (4 + 2 * (dimensionY - 1)) * i;
				//Different cases depending on i even or odd
				if (i == dimensionX-1)
				{
					if (dimensionX % 2 == 0)
					{
						boost::add_edge(toFinish - dimensionY * 2 - 1, toFinish - dimensionY * 4 - 2, *graph);
						int toStart = toFinish - (dimensionY * 2);
						for (int j = 0; j < dimensionY * 2; j++)
						{
							//One edge case
							if (j % 2 == 0)
							{
								boost::add_edge(toStart + j, toStart + j - 1, *graph);
							}
							//Two edge case
							else
							{
								boost::add_edge(toStart + j, toStart + j - 1, *graph);
								boost::add_edge(toStart + j, toStart + j - (dimensionY * 2 + 1), *graph);
							}
						}
					}
					else
					{
						boost::add_edge(toFinish - dimensionY * 2 - 1, toFinish - dimensionY * 4 - 3, *graph);
						int toStart = toFinish - (dimensionY * 2);
						for (int j = 0; j < dimensionY * 2; j++)
						{
							//One edge case
							if (j % 2 == 0)
							{
								boost::add_edge(toStart + j, toStart + j - 1, *graph);
							}
							//Two edge case
							else
							{
								boost::add_edge(toStart + j, toStart + j - 1, *graph);
								boost::add_edge(toStart + j, toStart + j - (dimensionY * 2 + 1)-1, *graph);
							}
						}
					}
				}
				//Even case
				else if (i % 2 == 0)
				{
					//Bottom edge
					boost::add_edge(toFinish - dimensionY * 2-1, toFinish - dimensionY * 4 - 3, *graph);
					//Then we alternate adding either one or two edges
					//Vertex at which we start alternating these vertices
					int toStart = toFinish - dimensionY * 2;
					for (int j = 0; j < dimensionY*2+1; j++)
					{
						//One edge case
						if (j % 2 == 0)
						{
							boost::add_edge(toStart + j, toStart + j - 1, *graph);
						}
						//Two edge case
						else
						{
							boost::add_edge(toStart + j, toStart + j - 1, *graph);
							boost::add_edge(toStart + j, toStart + j - (dimensionY*2 +2), *graph);
						}
					}
				}
				//Odd case
				else
				{
					//Then we alternate adding either one or two edges
					//Vertex at which we start alternating these vertices
					int toStart = toFinish - dimensionY * 2;
					for (int j = 0; j < dimensionY * 2+1; j++)
					{
						//One edge case
						if (j % 2 == 1)
						{
							boost::add_edge(toStart + j, toStart + j - 1, *graph);
						}
						//Two edge case
						else
						{
							boost::add_edge(toStart + j, toStart + j - 1, *graph);
							boost::add_edge(toStart + j, toStart + j - (dimensionY * 2 + 2), *graph);
						}
					}
				}
			}
			//Assign vertex positions
			double internalAngle = (2.0 / 3.0) * M_PI;
			Context::vertexPosition upLeft(-0.5f, (float)sqrt(3.0/4.0));
			Context::vertexPosition upRight(0.5f, (float)sqrt(3.0 / 4.0));
			Context::vertexPosition current(0.0f, 0.0f);
			//j now refers to a column of vertices, ranging from 0 to dimensionX+1
			for (int j = 0; j < dimensionX + 1; j++)
			{
				int startIndex;
				if (j == dimensionX)
				{
					startIndex = nVertices - 2 * dimensionY - 1;
					if (j % 2 == 1)
					{
						current.second = 0;
						current.first = (float)(1 + 3 * ((j - 1) / 2));
					}
					else
					{
						current.second = upLeft.second;
						current.first = (float)(3 * (j / 2) - 0.5);
					}
					(*vertexPositions)[startIndex] = current;
					for (int i = 1; i < dimensionY * 2 + 1; i++)
					{
						if (i % 2)
						{
							current.first += upRight.first; current.second += upRight.second;
							(*vertexPositions)[startIndex + i] = current;
						}
						else
						{
							current.first += upLeft.first; current.second += upLeft.second;
							(*vertexPositions)[startIndex + i] = current;
						}
					}
				}
				else
				{
					if (j == 0)
					{
						current.first = current.second = 0;
						startIndex = 0;
					}
					else if (j % 2 == 1)
					{
						current.first = (float)(((j - 1) / 2) * 3 + 1);
						current.second = 0;
						startIndex = 2 * dimensionY + (j - 1)*(dimensionY * 2 + 2) + 1;
					}
					else
					{
						current.first = (float)((j / 2) * 3);
						current.second = 0;
						startIndex = 2 * dimensionY + (j - 1)*(dimensionY * 2 + 2) + 1;
					}
					(*vertexPositions)[startIndex] = current;
					for (int i = 1; i < dimensionY * 2 + 2; i++)
					{
						if ((i % 2) ^ (!(j % 2)))
						{
							current.first += upRight.first; current.second += upRight.second;
							(*vertexPositions)[startIndex + i] = current;
						}
						else
						{
							current.first += upLeft.first; current.second += upLeft.second;
							(*vertexPositions)[startIndex + i] = current;
						}
					}
				}
			}
		}
	}
}