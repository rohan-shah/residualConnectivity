#ifndef CONTEXT_HEADER_GUARD
#define CONTEXT_HEADER_GUARD
#include <boost/graph/adjacency_list.hpp>
#include <vector>
#include <map>
#include <boost/shared_array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include "includeNumerics.h"
namespace discreteGermGrain
{
	struct vertexDistanceCache
	{
		//index of last vertex which is closer than or equal to some specified distance from current vertex
		int lastVertex;
		template<class Archive> void serialize(Archive& ar, const unsigned int version)
		{
			ar & lastVertex;
		}
	};
	enum
	{
		UNFIXED_OFF = 1, UNFIXED_ON = 2, FIXED_ON = 4, FIXED_OFF = 8, SUBPOINT = 16, 
		ON_MASK = 22, FIXED_MASK = 28, UNFIXED_MASK = 3
	};
	struct vertexState
	{
		int state;
		static vertexState fixed_off()
		{
			vertexState ret;
			ret.state = FIXED_OFF;
			return ret;
		};
		static vertexState unfixed_off()
		{
			vertexState ret;
			ret.state = UNFIXED_OFF;
			return ret;
		};
		template<class Archive> void serialize(Archive& ar, const unsigned int version)
		{
			ar & state;
		};
	};
	class Context : public boost::noncopyable
	{
	public:
		friend class boost::serialization::access;
		typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> inputGraph;
		typedef std::pair<float, float> vertexPosition;
		Context(boost::shared_ptr<const inputGraph> graph, boost::shared_ptr<const std::vector<int> > ordering, boost::shared_ptr<std::vector<vertexPosition> > vertexPositions, mpfr_class opProbability);
		Context(Context&& other);
		const std::vector<vertexDistanceCache>& getDistanceCache() const;
		template<class Archive> void save(Archive& ar, const unsigned int version) const
		{
			ar & graph;
			ar & ordering;
			ar & opProbability;
			
			std::size_t nVertices = boost::num_vertices(*graph);
			ar & boost::serialization::make_array(shortestDistances.get(), nVertices * nVertices);
		};
		template<class Archive> void load(Archive& ar, const unsigned int version)
		{
			ar & graph;
			ar & ordering;
			ar & opProbability;
			opProbabilityD = opProbability.convert_to<double>();

			std::size_t nVertices = boost::num_vertices(*graph);
			boost::shared_array<int> shortestDistances(new int[nVertices * nVertices]);
			ar & boost::serialization::make_array(shortestDistances.get(), nVertices * nVertices);
			this->shortestDistances = shortestDistances;
		};
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		std::size_t nVertices() const;
		const int* getShortestDistances() const;
		const inputGraph& getGraph() const;
		const std::vector<vertexDistanceCache>& getDistanceCache(int scale) const;
		static Context gridContext(int gridDimension, mpfr_class opProbability);
		static Context torusContext(int dimension, mpfr_class opProbability);
		static Context fromFile(std::string path, mpfr_class opProbability, bool& successful, std::string& message);
		Context& operator=(Context&& other);
		const std::vector<vertexPosition>& getVertexPositions() const;
		mpfr_class getOperationalProbability() const;
		double getOperationalProbabilityD() const;
	private:
		Context();
		mpfr_class opProbability;
		double opProbabilityD;
		boost::shared_ptr<const inputGraph> graph;

		boost::shared_ptr<const std::vector<int> > ordering;
		boost::shared_array<const int> shortestDistances;
		boost::shared_ptr<std::vector<vertexPosition> > vertexPositions;
	};
}
#endif
