#include "mex.h"
#include "monteCarloMethods/conditionalMC.h"
#include "convertGraph.h"
#include "exceptionHandling.h"
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray *prhs[])
{
BEGIN_MEX_WRAPPER
	//We need three inputs
	if(nrhs != 4 && nrhs != 5)
	{
		throw std::runtime_error("Function conditionalMCImpl requires four or five inputs");
	}
	//Only one output
	if(nlhs > 1)
	{
		throw std::runtime_error("Function conditionalMCImpl returns only one value");
	}
	//First input must be a probability
	bool probabilityValid = !mxIsComplex(prhs[0]) && !mxIsSparse(prhs[0]) && mxIsNumeric(prhs[0]) && mxGetNumberOfDimensions(prhs[0]) == 2;
	if(!probabilityValid)
	{
		throw std::runtime_error("First input must be a real number");
	}
	std::vector<residualConnectivity::mpfr_class> opProbabilities;
	for (std::size_t i = 0; i < mxGetNumberOfElements(prhs[0]); i++)
	{
		double probability = *(mxGetPr(prhs[0]) + i);
		if (probability < 0 || probability > 1)
		{
			throw std::runtime_error("Input probabilities must be between 0 and 1");
		}
		opProbabilities.push_back(probability);
	}

	//Second input must be the sample size n
	bool nValid = !mxIsComplex(prhs[1]) && !mxIsSparse(prhs[1]) && mxIsNumeric(prhs[1]) && mxGetNumberOfDimensions(prhs[1]) == 2 && mxGetNumberOfElements(prhs[1]) == 1;
	#define SECOND_INPUT_ERROR throw std::runtime_error("Second input must be a positive integer")
	if(!nValid) SECOND_INPUT_ERROR;
	double nDouble = mxGetScalar(prhs[1]);
	int n = (int)round(nDouble);
	if(fabs(n - nDouble) > 1e-5 || n <= 0) SECOND_INPUT_ERROR;

	//Third input must be the random seed
	bool seedValid = !mxIsComplex(prhs[2]) && !mxIsSparse(prhs[2]) && mxIsNumeric(prhs[2]) && mxGetNumberOfDimensions(prhs[2]) == 2 && mxGetNumberOfElements(prhs[2]) == 1;
	#define THIRD_INPUT_ERROR throw std::runtime_error("Third input must be a non-negative integer")
	if(!seedValid) THIRD_INPUT_ERROR;
	double seedDouble = mxGetScalar(prhs[2]);
	int seed = (int)round(seedDouble);
	if(fabs(seed - seedDouble) > 1e-5 || n <= 0) THIRD_INPUT_ERROR;
	
	boost::shared_ptr<residualConnectivity::context::inputGraph> graph(new residualConnectivity::context::inputGraph());
	boost::shared_ptr<std::vector<residualConnectivity::context::vertexPosition> > vertexPositions(new std::vector<residualConnectivity::context::vertexPosition>());
	if(nrhs == 4)
	{
		std::string error;
		bool conversionResult = convertGraphNoPositions(prhs[3], *graph, *vertexPositions, error);
		if(!conversionResult) throw std::runtime_error(error);
	}
	else
	{
		std::string error;
		bool conversionResult = convertGraph(prhs[3], prhs[4], *graph, *vertexPositions, error);
		if(!conversionResult) throw std::runtime_error(error);
	}
	boost::shared_ptr<std::vector<int> > ordering(new std::vector<int>(boost::num_vertices(*graph)));
	for(int i = 0; i < (int)boost::num_vertices(*graph); i++) (*ordering)[i] = i;
	residualConnectivity::context contextObj = residualConnectivity::context(graph, ordering, vertexPositions, opProbabilities);

	boost::mt19937 randomSource;
	randomSource.seed(seed);
	residualConnectivity::conditionalMCArgs args(contextObj, randomSource);
	args.n = n;

	residualConnectivity::conditionalMC(args);
	plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
	double* output = mxGetPr(plhs[0]);
	*output = args.estimate.convert_to<double>();
	return;
END_MEX_WRAPPER
}
