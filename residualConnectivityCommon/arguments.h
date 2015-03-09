#ifndef ARGUMENTS_HEADER_GUARD
#define ARGUMENTS_HEADER_GUARD
#include <boost/program_options/variables_map.hpp>
#include "Context.h"
#include <boost/random/mersenne_twister.hpp>
namespace discreteGermGrain
{
	bool readN(boost::program_options::variables_map& map, int& out);
	bool readProbability(boost::program_options::variables_map& variableMap, double& out, std::string& message);
	bool readGridGraph(boost::program_options::variables_map& variableMap, int& gridDimension, std::string& message);
	bool readContext(boost::program_options::variables_map& variableMap, Context& out, double opProbability, std::string& message);
	void readSeed(boost::program_options::variables_map& variableMap, boost::mt19937& randomSource);
	bool readInitialRadius(boost::program_options::variables_map& variableMap, int& out);
	bool readNGraphs(boost::program_options::variables_map& variableMap, int& out);
}
#endif
