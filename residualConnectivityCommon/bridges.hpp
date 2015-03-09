#ifndef BOOST_GRAPH_BRIDGES_HPP
#define BOOST_GRAPH_BRIDGES_HPP

#include <stack>
#include <vector>
#include <algorithm> // for std::min and std::max
#include <boost/config.hpp>
#include <boost/limits.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/concept/assert.hpp>
#include <boost/assert.hpp>

namespace boost
{
  namespace detail
  {
    template<typename ComponentMap, typename DiscoverTimeMap,
             typename LowPointMap, typename HighPointMap, typename DescendantsMap, typename PredecessorMap,
             typename Stack, typename IndexMap, typename DFSVisitor>
    struct bridges_visitor : public dfs_visitor<>
    {
      bridges_visitor
        (ComponentMap comp, std::size_t& c, 
         DiscoverTimeMap dtm,
         std::size_t& dfs_time, LowPointMap lowpt, HighPointMap highpt, DescendantsMap nd, PredecessorMap pred,
         Stack& S, IndexMap index_map,
         DFSVisitor vis)
          : comp(comp), c(c), 
            dtm(dtm), dfs_time(dfs_time), lowpt(lowpt),
			highpt(highpt), nd(nd),
            pred(pred), S(S), index_map(index_map),
            vis(vis) { }

      template <typename Vertex, typename Graph>
      void initialize_vertex(const Vertex& u, Graph& g)
      {
        put(pred, u, u);
        vis.initialize_vertex(u, g);
      }

		template <typename Vertex, typename Graph>
		void start_vertex(const Vertex& u, Graph& g)
		{
			vis.start_vertex(u, g);
		}

		template <typename Vertex, typename Graph>
		void discover_vertex(const Vertex& u, Graph& g)
		{
			std::size_t time = ++dfs_time;
			put(dtm, u, time);
			put(lowpt, u, time);
			put(highpt, u, time);
			put(nd, u, 1);
			vis.discover_vertex(u, g);
		}

		template <typename Edge, typename Graph>
		void examine_edge(const Edge& e, Graph& g)
		{
			vis.examine_edge(e, g);
		}

		template <typename Edge, typename Graph>
		void tree_edge(const Edge& e, Graph& g)
		{
			typename boost::graph_traits<Graph>::vertex_descriptor src = source(e, g);
			typename boost::graph_traits<Graph>::vertex_descriptor tgt = target(e, g);

			S.push(e);
			put(pred, tgt, src);
			put(nd, src, get(nd, src)+1);
			vis.tree_edge(e, g);
		}

      template <typename Edge, typename Graph>
      void back_edge(const Edge& e, Graph& g)
      {
        BOOST_USING_STD_MIN();
		BOOST_USING_STD_MAX();

        typename boost::graph_traits<Graph>::vertex_descriptor src = source(e, g);
        typename boost::graph_traits<Graph>::vertex_descriptor tgt = target(e, g);
        if ( tgt != get(pred, src) ) {
          S.push(e);
          put(lowpt, src,
              min BOOST_PREVENT_MACRO_SUBSTITUTION(get(lowpt, src),
                                                   get(dtm, tgt)));
		  put(highpt, src,
              max BOOST_PREVENT_MACRO_SUBSTITUTION(get(highpt, src),
                                                   get(dtm, tgt)));
        }
        vis.back_edge(e, g);
      }

      template <typename Edge, typename Graph>
      void forward_or_cross_edge(const Edge& e, Graph& g)
      {
        vis.forward_or_cross_edge(e, g);
      }

      template <typename Vertex, typename Graph>
      void finish_vertex(const Vertex& u, Graph& g)
      {
        BOOST_USING_STD_MIN();
		BOOST_USING_STD_MAX();
        Vertex parent = get(pred, u);
		put(lowpt, parent,
				min BOOST_PREVENT_MACRO_SUBSTITUTION(get(lowpt, parent),
													get(lowpt, u)));
		put(highpt, parent,
				max BOOST_PREVENT_MACRO_SUBSTITUTION(get(highpt, parent),
													get(highpt, u)));
        if(parent == u)
		{
			while (!S.empty() ) 
			{
				put(comp, S.top(), c);
				S.pop();
			}
			c++;
		}
		else if ( get(lowpt, u) == get(dtm, u) && get(highpt, u) < get(dtm, u) + get(nd, u)) 
		{
			//have we used up an additional identifier in this loop?
			bool usedAdditional = get(dtm, source(S.top(), g)) >= get(dtm, u);
			while ( get(dtm, source(S.top(), g)) >= get(dtm, u) ) 
			{
				put(comp, S.top(), c);
				S.pop();
			}
			if(usedAdditional) c++;
			put(comp, S.top(), c);
			BOOST_ASSERT (source(S.top(), g) == parent);
			BOOST_ASSERT (target(S.top(), g) == u);
			S.pop();
			c ++;
		}

        vis.finish_vertex(u, g);
      }

      ComponentMap comp;
      std::size_t& c;
      DiscoverTimeMap dtm;
      std::size_t& dfs_time;
      LowPointMap lowpt;
	  HighPointMap highpt;
	  DescendantsMap nd;
      PredecessorMap pred;
      Stack& S;
	  IndexMap index_map;
      DFSVisitor vis;
    };
  template<typename Graph, typename ComponentMap, typename VertexIndexMap, typename DiscoverTimeMap, typename LowPointMap, typename HighPointMap, typename DescendantsMap,
        typename PredecessorMap, typename DFSVisitor>
  void
    bridges_impl(const Graph & g, ComponentMap comp,
        VertexIndexMap index_map, DiscoverTimeMap dtm,
        LowPointMap lowpt, HighPointMap highpt, DescendantsMap nd, PredecessorMap pred, DFSVisitor dfs_vis)
  {
    typedef typename graph_traits<Graph>::vertex_descriptor vertex_t;
    typedef typename graph_traits<Graph>::edge_descriptor edge_t;
    BOOST_CONCEPT_ASSERT(( VertexListGraphConcept<Graph> ));
    BOOST_CONCEPT_ASSERT(( IncidenceGraphConcept<Graph> ));
    BOOST_CONCEPT_ASSERT(( WritablePropertyMapConcept<ComponentMap, edge_t> ));
    BOOST_CONCEPT_ASSERT(( ReadWritePropertyMapConcept<DiscoverTimeMap,
                                                  vertex_t> ));
    BOOST_CONCEPT_ASSERT(( ReadWritePropertyMapConcept<LowPointMap, vertex_t > ));
	BOOST_CONCEPT_ASSERT(( ReadWritePropertyMapConcept<HighPointMap, vertex_t > ));
	BOOST_CONCEPT_ASSERT(( ReadWritePropertyMapConcept<DescendantsMap, vertex_t > ));
    BOOST_CONCEPT_ASSERT(( ReadWritePropertyMapConcept<PredecessorMap,
                                                  vertex_t> ));

    std::size_t num_components = 0;
    std::size_t dfs_time = 0;
    std::stack<edge_t> S;

    detail::bridges_visitor<ComponentMap, DiscoverTimeMap,
        LowPointMap, HighPointMap, DescendantsMap, PredecessorMap, std::stack<edge_t>, 
        VertexIndexMap, DFSVisitor>
    vis(comp, num_components, dtm, dfs_time,
        lowpt, highpt, nd, pred, S, index_map, dfs_vis);

    depth_first_search(g, visitor(vis).vertex_index_map(index_map));

  }
  }
	template<typename Graph, typename ComponentMap>
	void bridges(const Graph & g, ComponentMap comp)
	{
		typedef typename graph_traits<Graph>::vertices_size_type vertices_size_type;
		typedef typename graph_traits<Graph>::vertex_descriptor vertex_t;

		std::vector<vertices_size_type> discover_time(num_vertices(g));
		std::vector<vertices_size_type> highpt(num_vertices(g));
		std::vector<vertices_size_type> lowpt(num_vertices(g));
		std::vector<vertices_size_type> pred(num_vertices(g));
		std::vector<vertices_size_type> nd(num_vertices(g));

		vertices_size_type vst(0);
		vertex_t vert = graph_traits<Graph>::null_vertex();
		auto index_map = get(vertex_index, g);
		detail::bridges_impl(g, comp, index_map, 
			make_iterator_property_map(discover_time.begin(), index_map, vst), 
			make_iterator_property_map(lowpt.begin(), index_map, vst),
			make_iterator_property_map(highpt.begin(), index_map, vst),
			make_iterator_property_map(nd.begin(), index_map, vst),
			make_iterator_property_map(pred.begin(), index_map, vert),
			make_dfs_visitor(null_visitor()));
	}

  
}// namespace boost

#endif
