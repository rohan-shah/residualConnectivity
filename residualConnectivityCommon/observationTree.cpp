#include "observationTree.h"
#include <igraph.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
namespace residualConnectivity
{
	observationTree::observationTree(boost::archive::binary_iarchive& ar)
		:externalContext(NULL)
	{
		ar >> *this;
	}
	observationTree::observationTree(boost::archive::text_iarchive& ar)
		:externalContext(NULL)
	{
		ar >> *this;
	}
	bool observationTree::layout() const
	{
		std::size_t totalVertices = 0;
		for(std::vector<observationCollection>::const_iterator i = levelData.begin(); i != levelData.end(); i++)
		{
			totalVertices += i->getSampleSize();
		}
		treeGraph.reset(new treeGraphType(totalVertices));
		std::size_t nLevels = levelData.size();

		//We need to construct an igraph for the purposes of laying out the graph. But then the igraph is deleted, only the boost graph is retained. 
		static bool attachedAttributes = false;
		if(!attachedAttributes)
		{
			igraph_i_set_attribute_table(&igraph_cattribute_table);
			attachedAttributes = true;
		}
		//Edges for the igraph
		igraph_vector_t igraphEdges;
		int igraphResult = igraph_vector_init(&igraphEdges, (long)totalVertices*2);
		if(igraphResult) return false;

		int igraphEdgeCounter = 0;
		
		//Set vertex properties, and also add edges
		treeGraphType::vertex_descriptor currentVertex = *(boost::vertices(*treeGraph).first);
		std::ptrdiff_t cumulativeVertices = 0, previousCumulativeVertices = 0;
		//-(std::ptrdiff_t)levelData[0].getSampleSize();
		for(std::size_t currentLevel = 0; currentLevel < nLevels; currentLevel++)
		{
			std::size_t currentLevelSize = levelData[currentLevel].getSampleSize();
			for(std::size_t currentLevelIndex = 0; currentLevelIndex < currentLevelSize; currentLevelIndex++)
			{
				(*treeGraph)[currentVertex].potentiallyDisconnected = potentiallyDisconnected[currentLevel][currentLevelIndex];
				(*treeGraph)[currentVertex].level = (int)currentLevel;
				(*treeGraph)[currentVertex].index = (int)currentLevelIndex;
				if(parentData[currentLevel][currentLevelIndex] >= 0)
				{
					boost::add_edge(previousCumulativeVertices + parentData[currentLevel][currentLevelIndex], currentVertex, *treeGraph);
					igraph_vector_set(&igraphEdges, igraphEdgeCounter, (igraph_real_t)(previousCumulativeVertices + parentData[currentLevel][currentLevelIndex]));
					igraph_vector_set(&igraphEdges, igraphEdgeCounter+1, (igraph_real_t)currentVertex);
					igraphEdgeCounter += 2;
				}
				currentVertex++;
			}
			previousCumulativeVertices = cumulativeVertices;
			cumulativeVertices += currentLevelSize;
		}

		igraph_t igraph;
		igraphResult = igraph_create(&igraph, &igraphEdges, 0, true);
			if(igraphResult) return false;

			igraph_matrix_t igraphCoordinates;
			igraphResult = igraph_matrix_init(&igraphCoordinates, 0, 0);
				if(igraphResult) return false;

				igraph_vector_t igraphRootVertices;
				int nRootVertices = (int)levelData[0].getSampleSize();
				igraphResult = igraph_vector_init(&igraphRootVertices, nRootVertices);
					if(igraphResult) return false;
					for(int i = 0; i < nRootVertices; i++) igraph_vector_set(&igraphRootVertices, i, i);

					igraphResult = igraph_layout_reingold_tilford(&igraph, &igraphCoordinates, IGRAPH_ALL, &igraphRootVertices, NULL);
					if(igraphResult) return false;
					for(int currentVertex = 0; currentVertex < cumulativeVertices; currentVertex++)
					{
						(*treeGraph)[currentVertex].x = MATRIX(igraphCoordinates, currentVertex, 0);
						(*treeGraph)[currentVertex].y = - MATRIX(igraphCoordinates, currentVertex, 1);
					}
				igraph_vector_destroy(&igraphRootVertices);
			igraph_matrix_destroy(&igraphCoordinates);
		igraph_destroy(&igraph);
		igraph_vector_destroy(&igraphEdges);

		perLevelVertexIdsFromGraph();
		return true;
	}
	const std::vector<std::vector<int > >& observationTree::getPerLevelVertexIds() const
	{
		return perLevelVertexIds;
	}
	void observationTree::perLevelVertexIdsFromGraph() const
	{
		std::size_t nLevels = levelData.size();
		perLevelVertexIds.clear();
		perLevelVertexIds.resize(nLevels);
		for(std::size_t level = 0; level < nLevels; level++)
		{
			perLevelVertexIds[level].resize(levelData[level].getSampleSize());
		}
		treeGraphType::vertex_iterator currentVertexIterator, endVertexIterator;
		boost::tie(currentVertexIterator, endVertexIterator) = boost::vertices(*treeGraph);
		for(; currentVertexIterator != endVertexIterator; currentVertexIterator++)
		{
			perLevelVertexIds[(*treeGraph)[*currentVertexIterator].level][(*treeGraph)[*currentVertexIterator].index] = (int)*currentVertexIterator;
		}
	}
	const Context& observationTree::getContext() const
	{
		if(externalContext) return *externalContext;
		return *containedContext;
	}
	observationTree::observationTree(Context const* externalContext, int initialRadius)
		:externalContext(externalContext), initialRadius(initialRadius)
	{
		std::size_t nLevels = initialRadius+1;
		parentData.resize(nLevels);
		potentiallyDisconnected.resize(nLevels);
		for(unsigned int i = 0; i < nLevels; i++)
		{
			observationCollection currentLevelData(externalContext, initialRadius-i);
			levelData.push_back(std::move(currentLevelData));
		}
	}
	void observationTree::reserve(std::size_t reservePerLevel)
	{
		std::size_t nLevels = levelData.size();
		for(std::size_t i = 0; i < nLevels; i++)
		{
			levelData[i].reserve(reservePerLevel);
			parentData[i].reserve(reservePerLevel);
			potentiallyDisconnected[i].reserve(reservePerLevel);
		}
	}
	std::size_t observationTree::nLevels() const
	{
		return levelData.size();
	}
	std::size_t observationTree::getSampleSize(unsigned int level) const
	{
		return levelData[level].getSampleSize();
	}
	void observationTree::expand(boost::shared_array<vertexState> state, unsigned int level, unsigned int index) const
	{
		if(level >= levelData.size())
		{
			throw std::runtime_error("Specified level does not exist in call to observationTree::expand");
		}
		if(index >= levelData[level].getSampleSize())
		{
			throw std::runtime_error("Specified observation does not exist in specified level, in call to observationTree::expand");
		}
		levelData[level].expand(index, state);
	}
	void observationTree::add(const observation& obs, unsigned int level, int parentIndex, bool potentiallyDisconnected)
	{
		//Graph will need to be laid out again
		if(treeGraph) treeGraph.reset();
		if(level >= levelData.size())
		{
			throw std::runtime_error("Specified level does not exist in call to observationTree::add");
		}
		levelData[level].add(obs);
		parentData[level].push_back(parentIndex);
		this->potentiallyDisconnected[level].push_back(potentiallyDisconnected);
	}
	void observationTree::vectorsFromGraph()
	{
		assert(treeGraph);
		treeGraphType::vertex_iterator current, end;
		boost::tie(current, end) = boost::vertices(*treeGraph);

		std::size_t nLevels = levelData.size();
		potentiallyDisconnected.clear();
		parentData.clear();
		potentiallyDisconnected.resize(nLevels);
		parentData.resize(nLevels);

		for(std::size_t level = 0; level < nLevels; level++)
		{
			parentData[level].resize(levelData[level].getSampleSize());
			potentiallyDisconnected[level].resize(levelData[level].getSampleSize());
		}
		for(; current != end; current++)
		{
			//This one we have to actually get the index of the parent vertex
			treeGraphType::in_edge_iterator current_in, end_in;
			boost::tie(current_in, end_in) = boost::in_edges(*current, *treeGraph);
			int level = (*treeGraph)[*current].level;
			int index = (*treeGraph)[*current].index;
			if(current_in == end_in) parentData[level][index] = -1;
			else parentData[level][index] = (*treeGraph)[current_in->m_source].index;
			//This one is just a lookup
			potentiallyDisconnected[level][(*treeGraph)[*current].index] = (*treeGraph)[*current].potentiallyDisconnected;
		}
		perLevelVertexIdsFromGraph();
	}
	const observationTree::treeGraphType& observationTree::getTreeGraph() const
	{
		if(!treeGraph) 
		{
			//This returns true in the case of graphviz not available
			bool result = layout();
			if(!result)
			{
				//This path is taken if graphviz is available but the layout fails
				throw std::runtime_error("Call to observationTree::layout failed");
			}
		}
		if(!treeGraph)
		{
			throw std::runtime_error("Call to observationTree::getTreeGraph failed, possibly because graphviz was not available");
		}
		return *treeGraph;
	}
}
