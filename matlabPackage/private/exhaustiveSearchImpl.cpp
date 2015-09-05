#include "mex.h"
#include "exactMethods/exhaustiveSearch.h"
#include "convertGraph.h"
#include "exceptionHandling.h"
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray *prhs[])
{
BEGIN_MEX_WRAPPER
	//We need three inputs
	if(nrhs != 1)
	{
		throw std::runtime_error("Function exhaustiveSearchImpl requires one input");
	}
	//Only one output
	if(nlhs > 1)
	{
		throw std::runtime_error("Function exhaustiveSearchImpl returns only one value");
	}
	//First input must be an adjacency matrix
	boost::shared_ptr<discreteGermGrain::Context::inputGraph> graph(new discreteGermGrain::Context::inputGraph());
	boost::shared_ptr<std::vector<discreteGermGrain::Context::vertexPosition> > vertexPositions(new std::vector<discreteGermGrain::Context::vertexPosition>());
	std::string error;
	bool conversionResult = convertGraphNoPositions(prhs[0], *graph, *vertexPositions, error);
	if(!conversionResult) throw std::runtime_error(error);

	std::vector<discreteGermGrain::counterType> sizeCounters;
	bool result = discreteGermGrain::exhaustiveSearch(*graph, sizeCounters, error);
	if (!result) throw std::runtime_error(error);

	//Horrible interface. First convert to std::string, then C strings. 
	std::vector<std::string> countersToStrings;
	for(std::vector<discreteGermGrain::counterType>::iterator i = sizeCounters.begin(); i != sizeCounters.end(); i++)
	{
		std::stringstream ss;
		ss << *i;
		countersToStrings.push_back(ss.str());
	}

	std::vector<const char*> countersToCStrings;
	for(std::vector<std::string>::iterator i = countersToStrings.begin(); i != countersToStrings.end(); i++)
	{
		countersToCStrings.push_back(i->c_str());
	}
	mwSize returnedArraySize = sizeCounters.size();
	plhs[0] = mxCreateCharMatrixFromStrings(returnedArraySize, &(countersToCStrings[0]));
	return;
END_MEX_WRAPPER
}
