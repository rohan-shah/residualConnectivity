#ifndef CONVERT_MATLAB_GRAPH_HEADER_GUARD
#define CONVERT_MATLAB_GRAPH_HEADER_GUARD
#include "Context.h"
#include "mex.h"
bool convertGraphNoPositions(const mxArray *input, discreteGermGrain::Context::inputGraph& outputGraph, std::vector<discreteGermGrain::Context::vertexPosition>& positions, std::string& error);
bool convertGraph(const mxArray *inputGraph, const mxArray *inputPositions, discreteGermGrain::Context::inputGraph& outputGraph, std::vector<discreteGermGrain::Context::vertexPosition>& positions, std::string& error);
#endif
