#include "exactMethods/exhaustiveSearch.h"
namespace residualConnectivity
{
	bool exhaustiveSearch(const context::inputGraph& inputGraph, std::vector<counterType>& sizeCounters, std::string& error)
	{
		error = "";
		const std::size_t nVertices = boost::num_vertices(inputGraph);
		if(nVertices > 36)
		{
			error = "Maximum allowable number of vertices is 36.";
			return false;
		}
		const counterType maximumState = 1LL << nVertices;
	
		sizeCounters.resize(nVertices+1);
		std::fill(sizeCounters.begin(), sizeCounters.end(), 0);
		
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
				context::inputGraph::edge_iterator start, end;
				boost::tie(start, end) = boost::edges(inputGraph);
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
		return true;
	}
}
