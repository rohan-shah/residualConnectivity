#ifndef CONVERT_MATLAB_GRAPH_HEADER_GUARD
#define CONVERT_MATLAB_GRAPH_HEADER_GUARD
#include "context.h"
#include "mex.h"
bool convertGraphNoPositions(const mxArray *input, residualConnectivity::context::inputGraph& outputGraph, std::vector<residualConnectivity::context::vertexPosition>& positions, std::string& error);
bool convertGraph(const mxArray *inputGraph, const mxArray *inputPositions, residualConnectivity::context::inputGraph& outputGraph, std::vector<residualConnectivity::context::vertexPosition>& positions, std::string& error);
#endif
