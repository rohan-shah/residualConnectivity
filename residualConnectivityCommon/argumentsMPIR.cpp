#include "argumentsMPIR.h"
#include <iostream>
namespace discreteGermGrain
{
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
