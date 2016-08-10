#include <boost/graph/connected_components.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/adjacency_matrix.hpp>
#include <iostream>
#include <iomanip>
#include "context.h"
#include <string>
namespace residualConnectivity
{
	typedef long long counterType;
	bool exhaustiveSearch(const context::inputGraph& inputGraph, std::vector<counterType>& sizeCounters, std::string& error);
}
