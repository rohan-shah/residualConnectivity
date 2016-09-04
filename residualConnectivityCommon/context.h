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
#include "includeMPFRResidualConnectivity.h"
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
		context(boost::shared_ptr<const inputGraph> graph, boost::shared_ptr<const std::vector<int> > ordering, boost::shared_ptr<std::vector<vertexPosition> > vertexPositions, std::vector<mpfr_class>& opProbabilities);
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
			ar & opProbabilities;
			ar & identicalProbabilities;
			
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
			ar & opProbabilities;
			ar & identicalProbabilities;
			opProbabilitiesD.clear();
			std::transform(opProbabilities.begin(), opProbabilities.end(), std::back_inserter(opProbabilitiesD), std::bind(&mpfr_class::convert_to<double>, std::placeholders::_1));

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
		const int* getShortestDistances() const;
		const inputGraph& getGraph() const;
		static context fromFile(std::string path, std::vector<mpfr_class>& opProbabilities);
		context& operator=(context&& other);
		const std::vector<vertexPosition>& getVertexPositions() const;
		const std::vector<mpfr_class>& getOperationalProbabilities() const;
		const std::vector<double>& getOperationalProbabilitiesD() const;
		bool hasIdenticalProbabilities() const;
		static bool readGraph(std::string path, context::inputGraph& graph, std::vector<vertexPosition>& vertexPositions, std::vector<int>& ordering, std::string& message);
	private:
		bool identicalProbabilities;
		context();
		std::vector<mpfr_class> opProbabilities;
		std::vector<double> opProbabilitiesD;
		boost::shared_ptr<const inputGraph> graph;

		boost::shared_ptr<const std::vector<int> > ordering;
		boost::shared_array<const int> shortestDistances;
		boost::shared_ptr<std::vector<vertexPosition> > vertexPositions;
	};
}
#endif
