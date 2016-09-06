#include "argumentsMPFR.h"
#include <iostream>
#include "constructGraphs.h"
#include <functional>
namespace residualConnectivity
{
	bool readGraph(boost::program_options::variables_map& variableMap, context::inputGraph& graph, std::vector<context::vertexPosition>& vertexPositions, std::vector<int>& ordering, std::string& message)
	{
		if (variableMap.count("graphFile") + variableMap.count("gridGraph") + variableMap.count("torusGraph") + variableMap.count("hexagonalGrid") != 1)
		{
			message = "Please enter exactly one of `gridGraph', `graphFile', `hexagonalGrid' or `torusGraph'";
			return false;
		}
		else if (variableMap.count("graphFile") == 1)
		{
			std::string internalMessage;
			bool successful = context::readGraph(variableMap["graphFile"].as<std::string>(), graph, vertexPositions, ordering, internalMessage);
			if (!successful)
			{
				message = "Error reading graphml file. " + internalMessage;
				return false;
			}
			return true;
		}
		else if (variableMap.count("torusGraph") == 1)
		{
			int gridDimension = variableMap["torusGraph"].as<int>();
			if (gridDimension <= 0)
			{
				message = "Input `torusGraph' must be a positive integer";
				return false;
			}
			constructGraphs::squareTorus(gridDimension, graph, vertexPositions);
			return true;
		}
		else if (variableMap.count("hexagonalGrid") == 1)
		{
			std::vector<int> gridDimensions = variableMap["hexagonalGrid"].as<std::vector<int> >();
			if (gridDimensions.size() != 2 || gridDimensions[0] <= 0 || gridDimensions[1] <= 0)
			{
				message = "Input `hexagonalGrid' must be a pair of positive numbers";
				return false;
			}
			constructGraphs::hexagonalTiling(gridDimensions[1], gridDimensions[0], graph, vertexPositions);
			std::size_t nVertices = boost::num_vertices(graph);
			ordering.resize(nVertices);
			for (int i = 0; i < (int)nVertices; i++) ordering[i] = i;
			return true;
		}
		else
		{
			int gridDimension = variableMap["gridGraph"].as<int>();
			if (gridDimension <= 0)
			{
				message = "Input `gridGraph' must be a positive integer";
				return false;
			}
			constructGraphs::squareGrid(gridDimension, graph, vertexPositions);
			std::size_t nVertices = boost::num_vertices(graph);
			ordering.resize(nVertices);
			for (int i = 0; i < (int)nVertices; i++) ordering[i] = i;
			return true;
		}
	}
	bool readProbabilityString(boost::program_options::variables_map& variableMap, std::vector<mpfr_class>& out, std::string& message)
	{
		if(variableMap.count("opProbabilities") != 1)
		{
			message = "Please enter exactly one value for input `opProbabilities'";
			return false;
		}
		try
		{
			std::vector<std::string> inputs = variableMap["opProbabilities"].as<std::vector<std::string> >();
			out.clear();
			std::transform(inputs.begin(), inputs.end(), std::back_inserter(out), [](std::string x){return mpfr_class(x); });
		}
		catch(...)
		{
			message = "Error parsing input `opProbabilities' as a number"; 
			return false;
		}
		return true;
	}
	bool readSingleProbabilityString(boost::program_options::variables_map& variableMap, mpfr_class& out, std::string& message)
	{
		if (variableMap.count("opProbabilities") != 1)
		{
			message = "Please enter exactly one value for input `opProbabilities'";
			return false;
		}
		std::vector<std::string> inputs;
		try
		{
			inputs = variableMap["opProbabilities"].as<std::vector<std::string> >();
		}
		catch (...)
		{
			message = "Error parsing input `opProbabilities' as a number";
			return false;
		}
		if (inputs.size() != 1)
		{
			message = "More than one probability was input. This method requires only a single value";
			return false;
		}
		out = mpfr_class(inputs[0]);
		return true;
	}
}
