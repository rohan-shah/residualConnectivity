#ifndef DISCRETE_GERM_GRAIN_OBS_TREE_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_OBS_TREE_HEADER_GUARD
#include "observationCollection.h"
#include "observation.h"
#include <stdexcept>
#include "binaryDataSet.h"
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
namespace discreteGermGrain
{
	class observationTree : public boost::noncopyable
	{
	public:
		struct vertexProperty
		{
		public:
			bool potentiallyDisconnected;
			int level;
			int index;
			double x, y;
			friend class boost::serialization::access;
		private:
			BOOST_SERIALIZATION_SPLIT_MEMBER()
			template<class Archive> void save(Archive& ar, const unsigned int version) const
			{
				ar << potentiallyDisconnected << level << index << x << y;
			}
			template<class Archive> void load(Archive& ar, const unsigned int version)
			{
				ar >> potentiallyDisconnected >> level >> index >> x >> y;
			}
		};
		typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, vertexProperty, boost::no_property> treeGraphType;

		typedef boost::adjacency_list< > laidOutBoostGraph;
		friend class boost::serialization::access;
		//The levels go 0, 1, ..., nLevels - 1, with level 0 being the topmost level of the tree
		observationTree(Context const* externalContext, const std::vector<double>& thresholds);
		void reserve(std::size_t reservePerLevel);
		observationTree(boost::archive::binary_iarchive& ar);
		observationTree(boost::archive::text_iarchive& ar);
		void add(const observation& obs, unsigned int level, int parentIndex, bool potentiallyDisconnected);
		const Context& getContext() const;
		void expand(boost::shared_array<vertexState> state, unsigned int level, unsigned int index) const;
		std::size_t getSampleSize(unsigned int level) const;
		std::size_t nLevels() const;
		const treeGraphType& getTreeGraph() const;
		bool layout() const;
		const std::vector<std::vector<int > >& getPerLevelVertexIds() const;
		const std::vector<double>& getThresholds() const;
	private:
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		template<class Archive> void save(Archive& ar, const unsigned int version) const
		{
			std::string typeString = "observationTree";
			ar << typeString;
			ar << thresholds;
			if(containedContext)
			{
				ar << *containedContext.get();
			}
			else ar << *externalContext;
			std::size_t levelDataSize = levelData.size();
			ar << levelDataSize;
			for(std::vector<observationCollection>::const_iterator i = levelData.begin(); i != levelData.end(); i++)
			{
				ar << *i;
			}
			if(!treeGraph) layout();
			//Even after we try and lay it out, it might not be laid out due to no graphviz available. 
			bool hasTree = treeGraph;
			ar << hasTree;
			if(hasTree)
			{
				ar << *treeGraph;
			}
			else
			{
				ar << parentData << potentiallyDisconnected;
			}
			typeString = "observationTree_end";
			ar << typeString;
		}
		void vectorsFromGraph();
		void perLevelVertexIdsFromGraph() const;
		template<class Archive> void load(Archive& ar, const unsigned int version)
		{
			std::string typeString;
			ar >> typeString;
			if(typeString != "observationTree")
			{
				throw std::runtime_error("Incorrect type specifier");
			}
			ar >> thresholds;
			containedContext.reset(new Context(ar));
			std::size_t levelDataSize;
			ar >> levelDataSize;
			for(std::size_t counter = 0; counter < levelDataSize; counter++)
			{
				observationCollection newCollection(ar);
				levelData.push_back(std::move(newCollection));
			}
			bool hasTree;
			ar >> hasTree;
			if(hasTree)
			{
				treeGraph.reset(new treeGraphType());
				ar >> *treeGraph;
				vectorsFromGraph();
			}
			else
			{
				ar >> parentData >> potentiallyDisconnected;
			}
			ar >> typeString;
			if(typeString != "observationTree_end")
			{
				throw std::runtime_error("Incorrect type specifier");
			}
			//Check that vectors all have the same sizes
			std::size_t nLevels = levelData.size();
			if(nLevels != parentData.size() || nLevels != potentiallyDisconnected.size())
			{
				throw std::runtime_error("Inconsistent number of levels");
			}
			for(unsigned int i = 0; i < nLevels; i++)
			{
				std::size_t nObjects = levelData[i].getSampleSize();
				if(parentData[i].size() != nObjects || potentiallyDisconnected[i].size() != nObjects)
				{
					throw std::runtime_error("Inconsistent number of objects at some level");
				}
			}
		}
		//Binary encoding for every observation, at every level of the tree
		std::vector<observationCollection> levelData;
		//Values giving the parents of every observation, at every level of the tree
		std::vector<std::vector<int> > parentData;
		std::vector<std::vector<bool> > potentiallyDisconnected;
		mutable std::vector<std::vector<int> > perLevelVertexIds;
		std::shared_ptr<Context> containedContext;
		Context const* externalContext;
		std::vector<double> thresholds;
		mutable std::shared_ptr<treeGraphType> treeGraph;
	};
}
#endif
