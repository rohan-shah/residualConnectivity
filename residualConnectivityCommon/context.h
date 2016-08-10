#ifndef CONTEXT_HEADER_GUARD
#define CONTEXT_HEADER_GUARD
#include <boost/graph/adjacency_list.hpp>
#include <vector>
#include <map>
#include <boost/shared_array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/graph/adj_list_serialize.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include "includeNumerics.h"
#include "vertexState.h"
#include "serializeGMP.hpp"
namespace residualConnectivity
{
	class context : public boost::noncopyable
	{
	public:
		friend class boost::serialization::access;
		typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS> inputGraph;
		typedef std::pair<float, float> vertexPosition;
		context(boost::shared_ptr<const inputGraph> graph, boost::shared_ptr<const std::vector<int> > ordering, boost::shared_ptr<std::vector<vertexPosition> > vertexPositions, mpfr_class opProbability);
		context(context&& other);
		context(boost::archive::text_iarchive& ar)
		{
			ar >> *this;
		}
		context(boost::archive::binary_iarchive& ar)
		{
			ar >> *this;
		}
		template<class Archive> void save(Archive& ar, const unsigned int version) const
		{
			ar & *graph;
			ar & *ordering;
			ar & opProbability;
			
			std::size_t nVertices = boost::num_vertices(*graph);
			ar & boost::serialization::make_array(shortestDistances.get(), nVertices * nVertices);

			ar & *vertexPositions.get();
		};
		template<class Archive> void load(Archive& ar, const unsigned int version)
		{
			{
				boost::shared_ptr<inputGraph> graph(new inputGraph());
				ar & *graph;
				this->graph = graph;
			}
			{
				boost::shared_ptr<std::vector<int> > ordering(new std::vector<int>());
				ar & *ordering;
				this->ordering = ordering;
			}
			ar & opProbability;
			opProbabilityD = opProbability.convert_to<double>();

			std::size_t nVertices = boost::num_vertices(*graph);
			{
				boost::shared_array<int> shortestDistances(new int[nVertices * nVertices]);
				ar & boost::serialization::make_array(shortestDistances.get(), nVertices * nVertices);
				this->shortestDistances = shortestDistances;
			}
			{
				vertexPositions.reset(new std::vector<vertexPosition>());
				ar & *vertexPositions;
			}
		};
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		std::size_t nVertices() const;
		const int* getShortestDistances() const;
		const inputGraph& getGraph() const;
		static context gridContext(int gridDimension, mpfr_class opProbability);
		static context hexagonalGridcontext(int gridDimensionX, int gridDimensionY, mpfr_class opProbability);
		static context torusContext(int dimension, mpfr_class opProbability);
		static context fromFile(std::string path, mpfr_class opProbability, bool& successful, std::string& message);
		context& operator=(context&& other);
		const std::vector<vertexPosition>& getVertexPositions() const;
		const mpfr_class& getOperationalProbability() const;
		double getOperationalProbabilityD() const;
	private:
		context();
		mpfr_class opProbability;
		double opProbabilityD;
		boost::shared_ptr<const inputGraph> graph;

		boost::shared_ptr<const std::vector<int> > ordering;
		boost::shared_array<const int> shortestDistances;
		boost::shared_ptr<std::vector<vertexPosition> > vertexPositions;
	};
}
#endif
