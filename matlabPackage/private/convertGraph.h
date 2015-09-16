#ifndef CONVERT_MATLAB_GRAPH_HEADER_GUARD
#define CONVERT_MATLAB_GRAPH_HEADER_GUARD
#include "Context.h"
#include "mex.h"
bool convertGraphNoPositions(const mxArray *input, residualConnectivity::Context::inputGraph& outputGraph, std::vector<residualConnectivity::Context::vertexPosition>& positions, std::string& error);
bool convertGraph(const mxArray *inputGraph, const mxArray *inputPositions, residualConnectivity::Context::inputGraph& outputGraph, std::vector<residualConnectivity::Context::vertexPosition>& positions, std::string& error);
#endif
