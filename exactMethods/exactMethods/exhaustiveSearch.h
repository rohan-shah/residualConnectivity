#include <boost/graph/connected_components.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <iostream>
#include <iomanip>
#include "Context.h"
#include <string>
namespace discreteGermGrain
{
	typedef long long counterType;
	bool exhaustiveSearch(const Context::inputGraph& inputGraph, std::vector<counterType>& sizeCounters, std::string& error);
}
