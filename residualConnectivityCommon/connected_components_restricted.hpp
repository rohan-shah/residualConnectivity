//
//=======================================================================
// Copyright 1997-2001 University of Notre Dame.
// Authors: Andrew Lumsdaine, Lie-Quan Lee, Jeremy G. Siek
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
//
#ifndef BOOST_GRAPH_CONNECTED_COMPONENTS_RESTRICTED_HPP
#define BOOST_GRAPH_CONNECTED_COMPONENTS_RESTRICTED_HPP

#include <boost/config.hpp>
#include "depth_first_search_restricted.hpp"
#include <boost/graph/properties.hpp>
#include <boost/graph/graph_concepts.hpp>
#include <boost/graph/overloading.hpp>
#include <boost/static_assert.hpp>
#include <boost/concept/assert.hpp>

namespace boost {

  // This function computes the connected components of an undirected
  // graph using a single application of depth first search.

/*  template <class Graph, class ComponentMap, class P, class T, class R>
  inline typename property_traits<ComponentMap>::value_type
  connected_components_restricted(const Graph& g, ComponentMap c, const std::vector<typename graph_traits<Graph>::vertex_descriptor>& initialVertices,
                       const bgl_named_params<P, T, R>& params
                       BOOST_GRAPH_ENABLE_IF_MODELS_PARM(Graph, vertex_list_graph_tag))
  {
    if (num_vertices(g) == 0) return 0;

    typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
    BOOST_CONCEPT_ASSERT(( WritablePropertyMapConcept<ComponentMap, Vertex> ));
    typedef typename boost::graph_traits<Graph>::directed_category directed;
    BOOST_STATIC_ASSERT((boost::is_same<directed, undirected_tag>::value));

    typedef typename property_traits<ComponentMap>::value_type comp_type;
    // c_count initialized to "nil" (with nil represented by (max)())
    comp_type c_count((std::numeric_limits<comp_type>::max)());
    detail::components_recorder<ComponentMap> vis(c, c_count);
    depth_first_search_restricted(g, initialVertices, params.visitor(vis));
    return c_count + 1;
  }*/

  template <class Graph, class ComponentMap, class ColorMap>
  inline typename property_traits<ComponentMap>::value_type
  connected_components_restricted(const Graph& g, ComponentMap c, ColorMap color, typename detail::depth_first_visit_restricted_impl_helper<Graph>::stackType& stack,
						const std::vector<typename graph_traits<Graph>::vertex_descriptor>& initialVertices
                       BOOST_GRAPH_ENABLE_IF_MODELS_PARM(Graph, vertex_list_graph_tag))
  {
    if (num_vertices(g) == 0) return 0;

    typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
    BOOST_CONCEPT_ASSERT(( WritablePropertyMapConcept<ComponentMap, Vertex> ));
    // BOOST_STATIC_ASSERT((boost::is_same<directed, undirected_tag>::value));

    typedef typename property_traits<ComponentMap>::value_type comp_type;
    // c_count initialized to "nil" (with nil represented by (max)())
    comp_type c_count((std::numeric_limits<comp_type>::max)());
    detail::components_recorder<ComponentMap> vis(c, c_count);
    depth_first_search_restricted(g, vis, color, stack, initialVertices);
    if(c_count == (std::numeric_limits<comp_type>::max)()) return 0;
    return c_count + 1;
  }

  
} // namespace boost

#ifdef BOOST_GRAPH_USE_MPI
#  include <boost/graph/distributed/connected_components.hpp>
#endif

#endif // BOOST_GRAPH_CONNECTED_COMPONENTS_HPP
