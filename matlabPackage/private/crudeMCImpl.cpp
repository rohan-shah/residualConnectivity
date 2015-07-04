#include "mex.h"
#include "crudeMCLib.h"
#include "convertGraph.h"
void mexFunction(int nlhs, mxArray* plhs[], int nrhs, const mxArray *prhs[])
{
	//We need three inputs
	if(nrhs != 3 && nrhs != 4)
	{
		mexErrMsgTxt("Function crudeMCImpl requires three or four inputs");
	}
	//Only one output
	if(nlhs > 1)
	{
		mexErrMsgTxt("Function crudeMCImpl returns only one value");
	}
	//First input must be a probability
	bool probabilityValid = !mxIsComplex(prhs[0]) && !mxIsSparse(prhs[0]) && mxIsNumeric(prhs[0]) && mxIsScalar(prhs[0]);
	if(!probabilityValid)
	{
		mexErrMsgTxt("First input must be a real number");
	}
	double probability = mxGetScalar(prhs[0]);
	if(probability < 0 || probability > 1)
	{
		mexErrMsgTxt("Input probability must be between 0 and 1");
	}
	mpfr_class probabilityMpfr = probability;

	//Second input must be the sample size n
	bool nValid = !mxIsComplex(prhs[1]) && !mxIsSparse(prhs[1]) && mxIsNumeric(prhs[1]) && mxIsScalar(prhs[1]);
	#define SECOND_INPUT_ERROR mexErrMsgTxt("Second input must be a positive integer")
	if(!nValid) SECOND_INPUT_ERROR;
	double nDouble = mxGetScalar(prhs[1]);
	int n = round(nDouble);
	if(fabs(n - nDouble) > 1e-5 || n <= 0) SECOND_INPUT_ERROR;
	
	boost::shared_ptr<discreteGermGrain::Context::inputGraph> graph(new discreteGermGrain::Context::inputGraph());
	boost::shared_ptr<std::vector<discreteGermGrain::Context::vertexPosition> > vertexPositions(new std::vector<discreteGermGrain::Context::vertexPosition>());
	if(nrhs == 3)
	{
		std::string error;
		bool conversionResult = convertGraphNoPositions(prhs[2], *graph, *vertexPositions, error);
		if(!conversionResult) mexErrMsgTxt(error.c_str());
	}
	else
	{
		std::string error;
		bool conversionResult = convertGraphNoPositions(prhs[2], prhs[3], *graph, *vertexPositions, error);
		if(!conversionResult) mexErrMsgTxt(error.c_str());
	}
	boost::shared_ptr<std::vector<int> > ordering(new std::vector<int>(boost::num_vertices(*graph)));
	for(int i = 0; i < (int)boost::num_vertices(*graph); i++) (*ordering)[i] = i;
	discreteGermGrain::Context context = discreteGermGrain::Context(graph, ordering, vertexPositions, probabilityMpfr);
	discreteGermGrain::crudeMCArgs args(context);

	plhs[0] = mxCreateDoubleMatrix(1, 1, mxREAL);
	double* output = mxGetPr(plhs[0]);
	*output = 0.1234;
	return;
}
