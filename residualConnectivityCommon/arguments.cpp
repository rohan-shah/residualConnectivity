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
	bool readProbability(boost::program_options::variables_map& variableMap, float& out, std::string& message)
	{
		if(variableMap.count("probability") != 1)
		{
			message = "Please enter a single value for input `probability'";
			return false;
		}
		float probability = variableMap["probability"].as<float>();
		if(probability > 1 || probability < 0)
		{
			message = "Please enter a number between 0 and 1 for probability";
			return false;
		}
		out = probability;
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
	bool readContext(boost::program_options::variables_map& variableMap, Context& out, std::string& message)
	{
		float opProbability;
		if(!readProbability(variableMap, opProbability, message))
		{
			return false;
		}
		if(variableMap.count("graphFile") + variableMap.count("gridGraph") + variableMap.count("torusGraph") != 1)
		{
			message = "Please enter exactly one of `gridGraph', `graphFile' or `torusGrahp'";
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
