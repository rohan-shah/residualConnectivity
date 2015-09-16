#include "argumentsMPFR.h"
#include <iostream>
namespace residualConnectivity
{
	bool readContext(boost::program_options::variables_map& variableMap, Context& out, mpfr_class opProbability, std::string& message)
	{
		if(variableMap.count("graphFile") + variableMap.count("gridGraph") + variableMap.count("torusGraph") + variableMap.count("hexagonalGrid") != 1)
		{
			message = "Please enter exactly one of `gridGraph', `graphFile', `hexagonalGrid' or `torusGraph'";
			return false;
		}
		else if(variableMap.count("graphFile") == 1)
		{
			bool successful;
			std::string internalMessage;
			out = Context::fromFile(variableMap["graphFile"].as<std::string>(), opProbability, successful, internalMessage);
			if(!successful)
			{
				message = "Error reading graphml file. " + internalMessage;
				return false;
			}
			return true;
		}
		else if(variableMap.count("torusGraph") == 1)
		{
			int gridDimension = variableMap["torusGraph"].as<int>();
			if(gridDimension <= 0)
			{
				message = "Input `torusGraph' must be a positive integer";
				return false;
			}
			out = Context::torusContext(gridDimension, opProbability);
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
			out = Context::hexagonalGridcontext(gridDimensions[1], gridDimensions[0], opProbability);
			return true;
		}
		else
		{
			int gridDimension = variableMap["gridGraph"].as<int>();
			if(gridDimension <= 0)
			{
				message = "Input `gridGraph' must be a positive integer";
				return false;
			}
			out = Context::gridContext(gridDimension, opProbability);
			return true;
		}
	}
	bool readProbabilityString(boost::program_options::variables_map& variableMap, mpfr_class& out, std::string& message)
	{
		if(variableMap.count("opProbability") != 1)
		{
			message = "Please enter exactly one value for input `opProbability'";
			return false;
		}
		try
		{
			out = mpfr_class(variableMap["opProbability"].as<std::string>());
		}
		catch(...)
		{
			message = "Error parsing input `opProbability' as a number"; 
			return false;
		}
		return true;
	}
}
