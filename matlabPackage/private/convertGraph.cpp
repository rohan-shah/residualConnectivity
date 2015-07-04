#include "convertGraph.h"
#define convertFunctions(CTYPE) 					\
bool convertToBinary_##CTYPE(int* output, const mxArray* input)		\
{									\
	int dim1 = mxGetM(input);					\
	int dim2 = mxGetN(input);					\
	CTYPE* data = (CTYPE *)mxGetData(input);			\
	for(int i = 0; i < dim1*dim2; i++)				\
	{								\
		if(data[i] != 0 && data[i] != 1) return false;		\
		output[i] = (int)data[i];				\
	}								\
	return true;							\
}									\
void convertToDouble_##CTYPE(double* output, const mxArray* input)	\
{									\
	int dim1 = mxGetM(input);					\
	int dim2 = mxGetN(input);					\
	CTYPE* data = (CTYPE *)mxGetData(input);			\
	for(int i = 0; i < dim1*dim2; i++)				\
	{								\
		output[i] = (double)data[i];				\
	}								\
}
convertFunctions(double)
convertFunctions(float)
convertFunctions(int8_t)
convertFunctions(uint8_t)
convertFunctions(int16_t)
convertFunctions(uint16_t)
convertFunctions(int32_t)
convertFunctions(uint32_t)
convertFunctions(int64_t)
convertFunctions(uint64_t)
#undef convertFunctions
#define callForBinaryConvert(MEXTYPE, CTYPE)		\
case MEXTYPE:					\
	return convertToBinary_##CTYPE(output, input);	\
	break;					
bool convertToBinary(int* output, const mxArray* input)
{
	switch(mxGetClassID(input))
	{
		callForBinaryConvert(mxDOUBLE_CLASS, double)
		callForBinaryConvert(mxSINGLE_CLASS, float)
		callForBinaryConvert(mxINT8_CLASS, int8_t)
		callForBinaryConvert(mxUINT8_CLASS, uint8_t)
		callForBinaryConvert(mxINT16_CLASS, int16_t)
		callForBinaryConvert(mxUINT16_CLASS, uint16_t)
		callForBinaryConvert(mxINT32_CLASS, int32_t)
		callForBinaryConvert(mxUINT32_CLASS, uint32_t)
		callForBinaryConvert(mxINT64_CLASS, int64_t)
		callForBinaryConvert(mxUINT64_CLASS, uint64_t)
	default:
		return false;
	}
}
#undef callForBinaryConvert
#define callForDoubleConvert(MEXTYPE, CTYPE)             \
	case MEXTYPE:                                   \
	convertToDouble_##CTYPE(output, input);  \
	break;
void convertToDouble(double* output, const mxArray* input)
{
	switch(mxGetClassID(input))
	{
		callForDoubleConvert(mxDOUBLE_CLASS, double)
		callForDoubleConvert(mxSINGLE_CLASS, float)
		callForDoubleConvert(mxINT8_CLASS, int8_t)
		callForDoubleConvert(mxUINT8_CLASS, uint8_t)
		callForDoubleConvert(mxINT16_CLASS, int16_t)
		callForDoubleConvert(mxUINT16_CLASS, uint16_t)
		callForDoubleConvert(mxINT32_CLASS, int32_t)
		callForDoubleConvert(mxUINT32_CLASS, uint32_t)
		callForDoubleConvert(mxINT64_CLASS, int64_t)
		callForDoubleConvert(mxUINT64_CLASS, uint64_t)
	default:
		break;
	}
}
#undef callForDoubleConvert
bool convertGraphNoPositions(const mxArray *input, discreteGermGrain::Context::inputGraph& outputGraph, std::vector<discreteGermGrain::Context::vertexPosition>& positions, std::string& error)
{
	//Input must be an adjacency matrix
	bool adjMatValid = !mxIsComplex(input) && !mxIsSparse(input) && mxIsNumeric(input);
	#define RETURN_ERROR {error = "Input adjMat must be a square binary matrix"; return false;}
	if(!adjMatValid) RETURN_ERROR
	int adjMatDim1 = mxGetM(input);
	int adjMatDim2 = mxGetN(input);
	if(adjMatDim1 != adjMatDim2) RETURN_ERROR

	std::vector<int> adjMatVector(adjMatDim1*adjMatDim2);
	bool conversionResult = convertToBinary(&(adjMatVector[0]), input);
	if(!conversionResult)
	{
		error = "Unable to convert binary matrix. Please check all values are either 0 or 1.";
		return false;
	}

	outputGraph = discreteGermGrain::Context::inputGraph(adjMatDim1);
	for(int i = 0; i < adjMatDim1; i++)
	{
		positions[i] = discreteGermGrain::Context::vertexPosition(i, 0);
		for(int j = 0; j < adjMatDim2; j++)
		{
			if(adjMatVector[i + adjMatDim1*j])
			{
				boost::add_edge(i, j, outputGraph);
			}
		}
	}
	return true;
#undef RETURN_ERROR
}
bool convertGraph(const mxArray *inputGraph, const mxArray *inputPositions, discreteGermGrain::Context::inputGraph& outputGraph, std::vector<discreteGermGrain::Context::vertexPosition>& positions, std::string& error)
{
	bool result = convertGraphNoPositions(inputGraph, outputGraph, positions, error);
	if(!result)
	{
		return false;
	}

	bool positionsValid = !mxIsComplex(inputPositions) && !mxIsSparse(inputPositions) && mxIsNumeric(inputPositions);
	#define RETURN_ERROR {error = "Input positions must be a numeric matrix with two columns"; return false;}
	if(!positionsValid) RETURN_ERROR
	int positionsDim1 = mxGetM(inputPositions);
	int positionsDim2 = mxGetN(inputPositions);
	if(positionsDim1 != 2) RETURN_ERROR
	
	//Convert matlab data to double
	std::vector<double> positionsVector(positionsDim1 * positionsDim2);
	convertToDouble(&(positionsVector[0]), inputPositions);

	//Now convert this into pairs
	for(int i = 0; i < positionsDim1; i++)
	{
		positions[i] = discreteGermGrain::Context::vertexPosition(positionsVector[i], positionsVector[i + positionsDim1]);
	}
	return true;
}
