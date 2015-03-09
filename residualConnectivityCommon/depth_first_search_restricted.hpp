//=======================================================================
// Copyright 1997, 1998, 1999, 2000 University of Notre Dame.
// Copyright 2003 Bruce Barr
// Authors: Andrew Lumsdaine, Lie-Quan Lee, Jeremy G. Siek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================

// Nonrecursive implementation of depth_first_visit_impl submitted by
// Bruce Barr, schmoost <at> yahoo.com, May/June 2003.
#ifndef BOOST_GRAPH_RECURSIVE_DFS_RESTRICTED_HPP
#define BOOST_GRAPH_RECURSIVE_DFS_RESTRICTED_HPP

#include <boost/graph/depth_first_search.hpp>

namespace boost {
  namespace detail {
	template<class IncidenceGraph> struct depth_first_visit_restricted_impl_helper
	{
		typedef typename graph_traits<IncidenceGraph>::vertex_descriptor Vertex;
		typedef typename graph_traits<IncidenceGraph>::out_edge_iterator Iter;
		typedef std::pair<Vertex, std::pair<Iter, Iter> > VertexInfo;
		typedef std::vector<VertexInfo> stackType;
	};
    template <class IncidenceGraph, class DFSVisitor, class ColorMap,
            class TerminatorFunc>
    void depth_first_visit_restricted_impl
      (const IncidenceGraph& g,
       typename graph_traits<IncidenceGraph>::vertex_descriptor u,
       DFSVisitor& vis,
       ColorMap color, typename depth_first_visit_restricted_impl_helper<IncidenceGraph>::stackType& stack, TerminatorFunc func = TerminatorFunc())
    {
      BOOST_CONCEPT_ASSERT(( IncidenceGraphConcept<IncidenceGraph> ));
      BOOST_CONCEPT_ASSERT(( DFSVisitorConcept<DFSVisitor, IncidenceGraph> ));
      typedef typename graph_traits<IncidenceGraph>::vertex_descriptor Vertex;
      BOOST_CONCEPT_ASSERT(( ReadWritePropertyMapConcept<ColorMap, Vertex> ));
      typedef typename property_traits<ColorMap>::value_type ColorValue;
      BOOST_CONCEPT_ASSERT(( ColorValueConcept<ColorValue> ));
      typedef color_traits<ColorValue> Color;
      typedef typename graph_traits<IncidenceGraph>::out_edge_iterator Iter;
      typedef std::pair<Vertex, std::pair<Iter, Iter> > VertexInfo;

      Iter ei, ei_end;
      stack.clear();

      // Possible optimization for vector
      //stack.reserve(num_vertices(g));

      typedef typename unwrap_reference<TerminatorFunc>::type TF;

      put(color, u, Color::gray());
      vis.discover_vertex(u, g);
      boost::tie(ei, ei_end) = out_edges(u, g);
      // Variable is needed to workaround a borland bug.
      TF& fn = static_cast<TF&>(func);
      if (fn(u, g)) {
          // If this vertex terminates the search, we push empty range
          stack.push_back(std::make_pair(u, std::make_pair(ei_end, ei_end)));
      } else {
          stack.push_back(std::make_pair(u, std::make_pair(ei, ei_end)));
      }
      while (!stack.empty()) {
        VertexInfo& back = stack.back();
        u = back.first;
        boost::tie(ei, ei_end) = back.second;
        stack.pop_back();
        while (ei != ei_end) {
          Vertex v = target(*ei, g);
          vis.examine_edge(*ei, g);
          ColorValue v_color = get(color, v);
          if (v_color == Color::white()) {
            vis.tree_edge(*ei, g);
            stack.push_back(std::make_pair(u, std::make_pair(++ei, ei_end)));
            u = v;
            put(color, u, Color::gray());
            vis.discover_vertex(u, g);
            boost::tie(ei, ei_end) = out_edges(u, g);
            if (fn(u, g)) {
                ei = ei_end;
            }
          } else if (v_color == Color::gray()) {
            vis.back_edge(*ei, g);
            ++ei;
          } else {
            vis.forward_or_cross_edge(*ei, g);
            ++ei;
          }
        }
        put(color, u, Color::black());
        vis.finish_vertex(u, g);
      }
    }
  } // namespace detail
  template <class VertexListGraph, class DFSVisitor, class ColorMap>
  void
  depth_first_search_restricted(const VertexListGraph& g, DFSVisitor vis, ColorMap color, typename detail::depth_first_visit_restricted_impl_helper<VertexListGraph>::stackType& stack,
                     const std::vector<typename graph_traits<VertexListGraph>::vertex_descriptor>& initialVertices)
  {
    typedef typename graph_traits<VertexListGraph>::vertex_descriptor Vertex;
    BOOST_CONCEPT_ASSERT(( DFSVisitorConcept<DFSVisitor, VertexListGraph> ));
    typedef typename property_traits<ColorMap>::value_type ColorValue;
    typedef color_traits<ColorValue> Color;

    typename graph_traits<VertexListGraph>::vertex_iterator ui, ui_end;
    for (boost::tie(ui, ui_end) = vertices(g); ui != ui_end; ++ui) {
      Vertex u = implicit_cast<Vertex>(*ui);
      if(get(color, u) == Color::white()) vis.initialize_vertex(u, g);
    }

    for (typename std::vector<Vertex>::const_iterator i = initialVertices.begin(); i != initialVertices.end(); i++) {
      Vertex u = implicit_cast<Vertex>(*i);
      ColorValue u_color = get(color, u);
      if (u_color == Color::white()) {       vis.start_vertex(u, g);
        detail::depth_first_visit_restricted_impl(g, u, vis, color, stack, detail::nontruth2());
      }
    }
  }

  // Boost.Parameter named parameter variant
  /*namespace graph {
    namespace detail {
      template <typename Graph>
      struct depth_first_search_restricted_impl {
        typedef void result_type;
        template <typename ArgPack>
        void operator()(const Graph& g, const ArgPack& arg_pack) const {
          using namespace boost::graph::keywords;
          boost::depth_first_search_restricted(g,
                                    arg_pack[_visitor | make_dfs_visitor(null_visitor())],
                                    boost::detail::make_color_map_from_arg_pack(g, arg_pack),
                                    arg_pack[_root_vertex || boost::detail::get_default_starting_vertex_t<Graph>(g)]);
        }
      };
    }
    BOOST_GRAPH_MAKE_FORWARDING_FUNCTION(depth_first_search_restricted, 1, 4)
  }

  BOOST_GRAPH_MAKE_OLD_STYLE_PARAMETER_FUNCTION(depth_first_search_restricted, 1)*/

  /*template <class IncidenceGraph, class DFSVisitor, class ColorMap>
  void depth_first_visit_restricted
    (const IncidenceGraph& g,
     typename graph_traits<IncidenceGraph>::vertex_descriptor u,
     DFSVisitor vis, ColorMap color)
  {
    vis.start_vertex(u, g);
    detail::depth_first_visit_restricted_impl(g, u, vis, color, detail::nontruth2());
  }

  template <class IncidenceGraph, class DFSVisitor, class ColorMap,
            class TerminatorFunc>
  void depth_first_visit_restricted
    (const IncidenceGraph& g,
     typename graph_traits<IncidenceGraph>::vertex_descriptor u,
     DFSVisitor vis, ColorMap color, TerminatorFunc func = TerminatorFunc())
  {
    vis.start_vertex(u, g);
    detail::depth_first_visit_restricted_impl(g, u, vis, color, func);
  }*/
} // namespace boost
#endif
