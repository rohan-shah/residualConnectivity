#include "context.h"
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include <boost/property_map/function_property_map.hpp>
#include <boost/graph/graphml.hpp>
#include <boost/property_map/vector_property_map.hpp>
#include "constructGraphs.h"
namespace residualConnectivity
{
	namespace contextImpl
	{
		template<typename Key, int Ret> class constant_property_map : public boost::put_get_helper<int, constant_property_map<Key, Ret> >
		{
		public:
			typedef Key key_type;
			typedef int reference;
			typedef int value_type;

			typedef boost::readable_property_map_tag category;

			constant_property_map(){}

			reference operator[](const Key&) const
			{
				return Ret;
			}
		};

		struct twoDArray
		{
			int* base;
			std::size_t dim;
			struct twoDArrayInternal
			{
				twoDArrayInternal(int* base)
					:base(base)
				{};
				int& operator[](std::size_t j)
				{
					return *(base + j);
				}
				const int& operator[](std::size_t j) const
				{
					return *(base + j);
				}
				int* base;
			};
			twoDArrayInternal operator[](std::size_t i) const
			{
				return twoDArrayInternal(base + dim*i);
			};
		};
	}
	context::context(boost::shared_ptr<const inputGraph> unorderedGraph, boost::shared_ptr<const std::vector<int> > ordering, boost::shared_ptr<std::vector<vertexPosition> > vertexPositions, std::vector<mpfr_class>& opProbabilities)
		: ordering(ordering), vertexPositions(new std::vector<vertexPosition>())
	{

		std::size_t nVertices = boost::num_vertices(*unorderedGraph);
		if (nVertices != ordering->size())
		{
			throw std::runtime_error("Graph ordering data had the wrong size");
		}
		if (opProbabilities.size() == 1)
		{
			opProbabilities.insert(opProbabilities.begin(), nVertices - 1, opProbabilities.front());
		}
		this->opProbabilities = opProbabilities;
		std::transform(opProbabilities.begin(), opProbabilities.end(), std::back_inserter(opProbabilitiesD), std::bind(&mpfr_class::convert_to<double>, std::placeholders::_1));
		
		std::vector<mpfr_class> copiedProbabilities(opProbabilities);
		std::sort(copiedProbabilities.begin(), copiedProbabilities.end());
		copiedProbabilities.erase(std::unique(copiedProbabilities.begin(), copiedProbabilities.end()), copiedProbabilities.end());
		identicalProbabilities = copiedProbabilities.size() == 1;

		if (nVertices != opProbabilities.size())
		{
			throw std::runtime_error("Vertex probabilities had the wrong size");
		}
		if (*std::min_element(ordering->begin(), ordering->end()) != 0)
		{
			throw std::runtime_error("Wrong minimum vertex in ordering");
		}
		if (*std::max_element(ordering->begin(), ordering->end()) != (int)nVertices - 1)
		{
			throw std::runtime_error("Wrong maximum vertex in ordering");
		}
		/*//construct inverse ordering vector, which tells us the order of the ith vertex in the unordered graph
		std::vector<int> orderPosition(nVertices);
		for(int i = 0; i < nVertices; i++)
		{
		orderPosition[(*ordering)[i]] = i;
		}*/

		//organise the vertices of the graph so that the ith vertex in the graph is the ith vertex under the ordering.
		boost::shared_ptr<inputGraph> orderedGraph(new inputGraph(nVertices));
		inputGraph::edge_iterator start, end;
		boost::tie(start, end) = boost::edges(*unorderedGraph);
		for (; start != end; start++)
		{
			boost::add_edge((*ordering)[start->m_source], (*ordering)[start->m_target], *orderedGraph);
		}

		this->graph = orderedGraph;
		//Rearrange vertex positions, too
		if (vertexPositions && vertexPositions->size() > 0)
		{
			if (nVertices != vertexPositions->size())
			{
				throw std::runtime_error("Vertex position data had the wrong size");
			}
			this->vertexPositions = boost::shared_ptr<std::vector<vertexPosition> >(new std::vector<vertexPosition>(nVertices));
			for (std::size_t i = 0; i < nVertices; i++) (*this->vertexPositions)[(*ordering)[i]] = (*vertexPositions)[i];
		}

		//get out shortest distances
		boost::shared_array<int> shortestDistances = boost::shared_array<int>(new int[nVertices*nVertices]);

		//set up structure that has we can use in the form tmp[i][j]. What a stupid notation, it should be operator()(i, j)
		contextImpl::twoDArray tmp;
		tmp.base = shortestDistances.get();
		tmp.dim = nVertices;
		//all edges have weight 1
		contextImpl::constant_property_map<context::inputGraph::edge_descriptor, 1> edgeWeights;
		boost::johnson_all_pairs_shortest_paths(*orderedGraph, tmp, boost::weight_map(edgeWeights));
		this->shortestDistances = shortestDistances;
	}
	std::size_t context::nVertices() const
	{
		return boost::num_vertices(*graph);
	}
	context::context(context&& other)
	{
		graph.swap(other.graph);
		ordering.swap(other.ordering);
		shortestDistances.swap(other.shortestDistances);
		vertexPositions.swap(other.vertexPositions);
		opProbabilities.swap(other.opProbabilities);
		opProbabilitiesD.swap(other.opProbabilitiesD);
		identicalProbabilities = other.identicalProbabilities;
	}
	const context::inputGraph& context::getGraph() const
	{
		return *graph;
	}
	const int* context::getShortestDistances() const
	{
		return shortestDistances.get();
	}
	context& context::operator=(context&& other)
	{
		graph.swap(other.graph);
		ordering.swap(other.ordering);
		shortestDistances.swap(other.shortestDistances);
		vertexPositions.swap(other.vertexPositions);
		opProbabilities.swap(other.opProbabilities);
		opProbabilitiesD.swap(other.opProbabilitiesD);
		identicalProbabilities = other.identicalProbabilities;
		return *this;
	}
	context::context()
	{}
	bool context::readGraph(std::string path, context::inputGraph& graph, std::vector<vertexPosition>& vertexPositions, std::vector<int>& ordering, std::string& message)
	{
		std::ifstream input(path);
		if (!input.is_open())
		{
			message = "Unable to open file";
			return false;
		}
		boost::dynamic_properties properties;

		boost::vector_property_map<int> orderingProperty;
		properties.property("order", orderingProperty);

		boost::vector_property_map<float> xProperty, yProperty;
		properties.property("x", xProperty);
		properties.property("y", yProperty);

		boost::read_graphml(input, graph, properties);
		std::size_t nVertices = boost::num_vertices(graph);

		ordering.insert(ordering.end(), orderingProperty.storage_begin(), orderingProperty.storage_end());

		for (auto xIterator = xProperty.storage_begin(), yIterator = yProperty.storage_begin(); xIterator != xProperty.storage_end(); xIterator++, yIterator++)
		{
			vertexPositions.push_back(vertexPosition(*xIterator, *yIterator));
		}

		//Assume the ordering is just the identity, if none was given
		if (ordering.size() == 0)
		{
			ordering.resize(nVertices);
			for (std::size_t i = 0; i < nVertices; i++) ordering[i] = (int)i;
		}
		if (ordering.size() != nVertices)
		{
			message = "Wrong number of vertices in ordering";
			return false;
		}
		if (vertexPositions.size() != 0 && vertexPositions.size() != nVertices)
		{
			message = "Vertex positions must be specified for all vertices or none";
			return false;
		}
		return true;
	}
	context context::fromFile(std::string path, std::vector<mpfr_class>& opProbabilities)
	{
		std::string message;
		boost::shared_ptr<context::inputGraph> graph(new context::inputGraph());
		boost::shared_ptr<std::vector<vertexPosition> > vertexPositions(new std::vector<vertexPosition>());
		boost::shared_ptr<std::vector<int> > ordering(new std::vector<int>());
		bool successful = context::readGraph(path, *graph.get(), *vertexPositions.get(), *ordering.get(), message);
		if (!successful) throw std::runtime_error(message);
		return context(graph, ordering, vertexPositions, opProbabilities);
	}
	const std::vector<context::vertexPosition>& context::getVertexPositions() const
	{
		return *vertexPositions;
	}
	const std::vector<mpfr_class>& context::getOperationalProbabilities() const
	{
		return opProbabilities;
	}
	const std::vector<double>& context::getOperationalProbabilitiesD() const
	{
		return opProbabilitiesD;
	}
	bool context::hasIdenticalProbabilities() const
	{
		return identicalProbabilities;
	}
}
