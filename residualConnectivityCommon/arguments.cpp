#include "arguments.h"
namespace discreteGermGrain
{
	bool readN(boost::program_options::variables_map& variableMap, int& out)
	{
		if(variableMap.count("n") != 1)
		{
			std::cout << "Please enter a single value for input `n'" << std::endl;
			return false;
		}
		out = variableMap["n"].as<int>();
		if(out <= 0)
		{
			std::cout << "Input `n' must be a positive integer" << std::endl;
			return false;
		}
		return true;
	}
	bool readGridGraph(boost::program_options::variables_map& variableMap, int& gridDimension, std::string& message)
	{
		if(variableMap.count("gridGraph") != 1)
		{
			message = "Please enter a single value for `input' gridGraph";
			return false;
		}
		gridDimension = variableMap["gridGraph"].as<int>();
		if(gridDimension <= 0)
		{
			message = "Input `gridGraph' must be a positive number";
			return false;
		}
		return true;
	}
	void readSeed(boost::program_options::variables_map& variableMap, boost::mt19937& randomSource)
	{
		if(variableMap.count("seed") > 0)
		{
			randomSource.seed(variableMap["seed"].as<int>());
		}
	}
	bool readInitialRadius(boost::program_options::variables_map& variableMap, int& out)
	{
		if(variableMap.count("initialRadius") != 1)
		{
			std::cout << "Please enter a single value for input `initialRadius'" << std::endl;
			return false;
		}
		out = variableMap["initialRadius"].as<int>();
		if(out < 0)
		{
			std::cout << "Input `initialRadius' must be nonnegative" << std::endl;
			return false;
		}
		return true;
	}
	bool readNGraphs(boost::program_options::variables_map& variableMap, int& out)
	{
		if(variableMap.count("nGraphs") != 1)
		{
			std::cout << "Please enter a single option value for input `nGraphs'" << std::endl;
			return false;
		}
		out = variableMap["nGraphs"].as<int>();
		if(out <= 0)
		{
			std::cout << "Input `nGraphs' must be positive" << std::endl;
			return false;
		}
		return true;
	}
}
