#include <boost/graph/connected_components.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <boost/program_options.hpp>
#include <boost/math/special_functions/binomial.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/sum_kahan.hpp>
#include <iostream>
#include <iomanip>
#include "Context.h"
#include "arguments.h"
namespace discreteGermGrain
{
	int main(int argc, char** argv)
	{
		boost::program_options::options_description options("Usage");
		options.add_options()
			("gridGraph", boost::program_options::value<int>(), "(int) The number of points for each dimension of the square grid. Incompatible with graphFile")
			("graphFile", boost::program_options::value<std::string>(), "(string) The path to a graphml file. Incompatible with gridGraph")
			("help", "Display this message");

		boost::program_options::variables_map variableMap;
		try
		{
			boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options), variableMap);
		}
		catch(boost::program_options::error& ee)
		{
			std::cerr << "Error parsing command line arguments: " << ee.what() << std::endl << std::endl;
			std::cerr << options << std::endl;
			return -1;
		}
		if(variableMap.count("help") > 0)
		{
			std::cout <<
				"This program exhaustively enumerates the subgraphs of the specified graph. It counts the number of connected subgraphs with k vertices for k <= nVertices. The results are human-readable and written to standard output. Computationally demanding, and currently limited to nVertices <= 36. See ExhaustiveProbability.exe. \n\n";
			std::cout << options << std::endl;
			return 0;
		}
		std::string message;
		Context context(Context::gridContext(1, 0));
		if(!readContext(variableMap, context, message))
		{
			std::cout << message << std::endl;
			return 0;
		}

		const std::size_t nVertices = boost::num_vertices(context.getGraph());
		if(nVertices > 36)
		{
			std::cerr << "Maximum allowable number of vertices is 36. Exiting...." << std::endl;
			return 0;
		}
		typedef long long counterType;
		const counterType maximumState = 1LL << nVertices;
	
		counterType* sizeCounters = new counterType[nVertices+1];
		memset(sizeCounters, 0, sizeof(counterType) * (nVertices+1));
		
		#pragma omp parallel 
		{		
			counterType* privateSizeCounters = new counterType[nVertices+1];
			memset(privateSizeCounters, 0, sizeof(counterType) * (nVertices+1));

			std::vector<int> connectedComponents(nVertices);
			std::vector<int> vertexIndices(nVertices);
			typedef boost::adjacency_matrix<boost::undirectedS> graphType;
			graphType graph(nVertices, boost::no_property());
		
			#pragma omp for
			for(counterType state = 0; state < maximumState; state++)
			{
				//first the vertices
				int nVerticesSubgraph = 0;
				for(int vertexCounter = 0; vertexCounter < (int)nVertices; vertexCounter++)
				{
					if(state & (1LL << vertexCounter)) 
					{
						vertexIndices[vertexCounter] = nVerticesSubgraph++;
					}
				}
				graph.m_matrix.clear();
				graph.m_matrix.resize(nVerticesSubgraph * (nVerticesSubgraph+ 1) / 2);
				graph.m_vertex_set = graphType::VertexList(0, nVerticesSubgraph);
				
				graph.m_vertex_properties.clear();
				graph.m_vertex_properties.resize(nVerticesSubgraph);
				graph.m_num_edges = 0;

				//now the edges
				Context::inputGraph::edge_iterator start, end;
				boost::tie(start, end) = boost::edges(context.getGraph());
				while(start != end)
				{
					if((state & (1LL << start->m_source)) && (state & (1LL << start->m_target)))
					{
						boost::add_edge(vertexIndices[start->m_source], vertexIndices[start->m_target], graph);
					}
					start++;
				}
				int nComponents = boost::connected_components(graph, &(connectedComponents[0]));
				if(nComponents <= 1)
				{
					privateSizeCounters[nVerticesSubgraph]++;
				}
			}
			#pragma omp critical
			{
				for(std::size_t i = 0; i < nVertices+1; i++)
				{
					sizeCounters[i] += privateSizeCounters[i];
				}
			}
			delete[] privateSizeCounters;
		}
		std::cout << "Number of connected subgraphs with that number of points" << std::endl;
		for(std::size_t i = 0; i < nVertices+1; i++)
		{
			std::cout << std::setw(3) << i << ":  " << sizeCounters[i] << std::endl;
		}
		delete[] sizeCounters;
		return 0;
	}
}
int main(int argc, char** argv)
{
	return discreteGermGrain::main(argc, argv);
}
