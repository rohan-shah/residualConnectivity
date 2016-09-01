#include "exactMethods/exhaustiveSearch.h"
namespace residualConnectivity
{
	bool exhaustiveSearchUnequalProbabilities(const context::inputGraph& inputGraph, std::vector<mpfr_class>& probabilities, mpfr_class& result, std::string& error)
	{
		error = "";
		const std::size_t nVertices = boost::num_vertices(inputGraph);
		if(nVertices > 36)
		{
			error = "Maximum allowable number of vertices is 36.";
			return false;
		}
		if(probabilities.size() != nVertices)
		{
			error = "Length of input probabilities was different from the number of vertices";
			return false;
		}
		std::vector<mpfr_class> complementaryProbabilities;
		for(std::vector<mpfr_class>::iterator i = probabilities.begin(); i != probabilities.end(); i++)
		{
			complementaryProbabilities.push_back(1 - *i);
		}
		const counterType maximumState = 1LL << nVertices;
	
		result = 0;
#ifdef USE_OPENMP
		#pragma omp parallel 
#endif
		{		
			mpfr_class privateResult = 0;

			std::vector<int> connectedComponents(nVertices);
			std::vector<int> vertexIndices(nVertices);
			typedef boost::adjacency_matrix<boost::undirectedS> graphType;
			graphType graph(nVertices, boost::no_property());
#ifdef USE_OPENMP
			#pragma omp for
#endif
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
					mpfr_class currentTerm = 1;
					for(int vertexCounter = 0; vertexCounter < (int)nVertices; vertexCounter++)
					{
						if(state & (1LL << vertexCounter)) currentTerm *= probabilities[vertexCounter];
						else currentTerm *= complementaryProbabilities[vertexCounter];
					}
					privateResult += currentTerm;
				}
			}
#ifdef USE_OPENMP
			#pragma omp critical
#endif
			{
				result += privateResult;
			}
		}
		return true;
	}
}
