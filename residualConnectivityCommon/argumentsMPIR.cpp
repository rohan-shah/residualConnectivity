#include "argumentsMPIR.h"
#include <iostream>
namespace discreteGermGrain
{
	bool readProbabilityString(boost::program_options::variables_map& variableMap, mpf_class& out)
	{
		if(variableMap.count("probability") != 1)
		{
			std::cout << "Please enter exactly one value for input `probability'" << std::endl;
			return false;
		}
		int retVal = mpf_set_str(out.get_mpf_t(), variableMap["probability"].as<std::string>().c_str(), 10);
		return retVal == 0;
	}
}
