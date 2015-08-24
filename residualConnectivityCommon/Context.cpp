#include "Context.h"
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include <boost/property_map/function_property_map.hpp>
#include <boost/graph/graphml.hpp>
#include <boost/property_map/vector_property_map.hpp>
#include "constructGraphs.h"
namespace discreteGermGrain
{
	namespace ContextImpl
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
	Context::Context(boost::shared_ptr<const inputGraph> unorderedGraph, boost::shared_ptr<const std::vector<int> > ordering, boost::shared_ptr<std::vector<vertexPosition> > vertexPositions, mpfr_class opProbability)
		: opProbability(opProbability), opProbabilityD(opProbability.convert_to<double>()), ordering(ordering), vertexPositions(new std::vector<vertexPosition>())
	{
		std::size_t nVertices = boost::num_vertices(*unorderedGraph);
		if(nVertices != ordering->size())
		{
			throw std::runtime_error("Graph ordering data had the wrong size");
		}
		if(*std::min_element(ordering->begin(), ordering->end()) != 0)
		{
			throw std::runtime_error("Wrong minimum vertex in ordering");
		}
		if(*std::max_element(ordering->begin(), ordering->end()) != (int)nVertices-1)
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
		for(;start != end; start++)
		{
			boost::add_edge((*ordering)[start->m_source], (*ordering)[start->m_target], *orderedGraph);
		}

		this->graph = orderedGraph;
		//Rearrange vertex positions, too
		if(vertexPositions && vertexPositions->size() > 0)
		{
			if(nVertices != vertexPositions->size())
			{
				throw std::runtime_error("Vertex position data had the wrong size");
			}
			this->vertexPositions = boost::shared_ptr<std::vector<vertexPosition> >(new std::vector<vertexPosition>(nVertices));
			for(std::size_t i = 0; i < nVertices; i++) (*this->vertexPositions)[(*ordering)[i]] = (*vertexPositions)[i];
		}

		//get out shortest distances
		boost::shared_array<int> shortestDistances = boost::shared_array<int>(new int[nVertices*nVertices]);

		//set up structure that has we can use in the form tmp[i][j]. What a stupid notation, it should be operator()(i, j)
		ContextImpl::twoDArray tmp;
		tmp.base = shortestDistances.get();
		tmp.dim = nVertices;
		//all edges have weight 1
		ContextImpl::constant_property_map<Context::inputGraph::edge_descriptor, 1> edgeWeights;
		boost::johnson_all_pairs_shortest_paths(*orderedGraph, tmp, boost::weight_map(edgeWeights));
		this->shortestDistances = shortestDistances;
	}
	std::size_t Context::nVertices() const
	{
		return boost::num_vertices(*graph);
	}
	Context::Context(Context&& other)
	{
		graph.swap(other.graph);
		ordering.swap(other.ordering);
		shortestDistances.swap(other.shortestDistances);
		vertexPositions.swap(other.vertexPositions);
		opProbability = other.opProbability;
		opProbabilityD = opProbability.convert_to<double>();
	}
	const Context::inputGraph& Context::getGraph() const
	{
		return *graph;
	}
	Context Context::gridContext(int gridDimension, mpfr_class opProbability)
	{
		boost::shared_ptr<std::vector<vertexPosition> > vertexPositions;
		boost::shared_ptr<Context::inputGraph> graph;
		constructGraphs::squareGrid(gridDimension, graph, vertexPositions);
		boost::shared_ptr<std::vector<int> > ordering(new std::vector<int>(gridDimension * gridDimension));
		for(int i = 0; i < gridDimension * gridDimension; i++) (*ordering)[i] = i;

		return Context(graph, ordering, vertexPositions, opProbability);
	}
	const int* Context::getShortestDistances() const
	{
		return shortestDistances.get();
	}
	Context& Context::operator=(Context&& other)
	{
		graph.swap(other.graph);
		ordering.swap(other.ordering);
		shortestDistances.swap(other.shortestDistances);
		vertexPositions.swap(other.vertexPositions);
		opProbability = other.opProbability;
		opProbabilityD = opProbability.convert_to<double>();
		return *this;
	}
	Context::Context()
	{}
	Context Context::fromFile(std::string path, mpfr_class opProbability, bool& successful, std::string& message)
	{
		std::ifstream input(path);
		if(!input.is_open())
		{
			message = "Unable to open file";
			successful = false;
			return Context();
		}
		boost::dynamic_properties properties;
		boost::shared_ptr<inputGraph> graph(new inputGraph());

		boost::vector_property_map<int> orderingProperty;
		properties.property("order", orderingProperty);

		boost::vector_property_map<float> xProperty, yProperty;
		properties.property("x", xProperty);
		properties.property("y", yProperty);

		boost::read_graphml(input, *graph, properties);
		std::size_t nVertices = boost::num_vertices(*graph);

		boost::shared_ptr<std::vector<int> > ordering(new std::vector<int>());
		ordering->insert(ordering->end(), orderingProperty.storage_begin(), orderingProperty.storage_end());
		
		boost::shared_ptr<std::vector<vertexPosition> > vertexPositions(new std::vector<vertexPosition>());
		for(auto xIterator = xProperty.storage_begin(), yIterator = yProperty.storage_begin(); xIterator != xProperty.storage_end(); xIterator++, yIterator++)
		{
			vertexPositions->push_back(vertexPosition(*xIterator, *yIterator));
		}

		successful = true;
		//Assume the ordering is just the identity, if none was given
		if(ordering->size() == 0)
		{
			ordering->resize(nVertices);
			for(std::size_t i = 0; i < nVertices; i++) (*ordering)[i] = (int)i;
		}
		if(ordering->size() != nVertices)
		{
			successful = false;
			message = "Wrong number of vertices in ordering";
			return Context();
		}
		if(vertexPositions->size() != 0 && vertexPositions->size() != nVertices)
		{
			successful = false;
			message = "Vertex positions must be specified for all vertices or none";
			return Context();
		}
		return Context(graph, ordering, vertexPositions, opProbability);
	}
	const std::vector<Context::vertexPosition>& Context::getVertexPositions() const
	{
		return *vertexPositions;
	}
	Context Context::torusContext(int dimension, mpfr_class opProbability)
	{
		boost::shared_ptr<std::vector<vertexPosition> > vertexPositions;
		boost::shared_ptr<Context::inputGraph> graph;
		constructGraphs::squareTorus(dimension, graph, vertexPositions);
		boost::shared_ptr<std::vector<int> > ordering(new std::vector<int>(dimension * dimension));
		for(int i = 0; i < dimension * dimension; i++) (*ordering)[i] = i;

		return Context(graph, ordering, vertexPositions, opProbability);
	}
	Context Context::hexagonalGridcontext(int gridDimensionX, int gridDimensionY, mpfr_class opProbability)
	{
		boost::shared_ptr<std::vector<vertexPosition> > vertexPositions;
		boost::shared_ptr<Context::inputGraph> graph;
		constructGraphs::hexagonalTiling(gridDimensionX, gridDimensionY, graph, vertexPositions);

		std::size_t nVertices = boost::num_vertices(*graph);
		boost::shared_ptr<std::vector<int> > ordering(new std::vector<int>(nVertices));
		for (int i = 0; i < (int)nVertices; i++) (*ordering)[i] = i;

		return Context(graph, ordering, vertexPositions, opProbability);
	}
	const mpfr_class& Context::getOperationalProbability() const
	{
		return opProbability;
	}
	double Context::getOperationalProbabilityD() const
	{
		return opProbabilityD;
	}
}
