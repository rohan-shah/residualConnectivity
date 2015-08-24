#ifndef ARGUMENTS_HEADER_GUARD
#define ARGUMENTS_HEADER_GUARD
#include <boost/program_options/variables_map.hpp>
#include "Context.h"
#include <boost/random/mersenne_twister.hpp>

#define GRID_GRAPH_OPTION ("gridGraph", boost::program_options::value<int>(), "(int) The dimension of the square grid graph to use. Incompatible with other graph specifiers. ")
#define GRAPH_FILE_OPTION ("graphFile", boost::program_options::value<std::string>(), "(string) The path to a graphml file. Incompatible with other graph specifiers. ")
#define TORUS_GRAPH_OPTION ("torusGraph", boost::program_options::value<int>(), "(int) The dimension of the torus graph to use. Incompatible with other graph specifiers. ")
#define HEXAGONAL_GRID_OPTION ("hexagonalGrid", boost::program_options::value<std::vector<int> >()->multitoken(), "(int, int) The dimensions of the hexagonal grid graph to use. Incompatible with other graph specifiers. ")
#define PROBABILITY_OPTION ("opProbability", boost::program_options::value<std::string>(), "(float) The probability that a vertex is open")
#define SEED_OPTION ("seed", boost::program_options::value<int>(), "(int) The random seed used to generate the random graphs")
#define INITIAL_RADIUS_OPTION ("initialRadius", boost::program_options::value<int>(), "(int) The initial radius")
#define N_OPTION ("n", boost::program_options::value<int>(), "(int) The number of graphs initially generated")
#define SPLITTING_FACTOR_OPTION ("splittingFactor", boost::program_options::value<std::vector<float> >()->multitoken(), "(float) The splitting factor to use at every step")
#define OUTPUT_TREE_OPTION ("outputTree", boost::program_options::value<std::string>(), "(string, optional) File to output tree to")
#define OUTPUT_DISTRIBUTION_OPTION ("outputDistribution", boost::program_options::value<std::string>(), "(string, optional) File to output realisations to")
#define EXPECTED_UP_NUMBER_OPTION ("expectedUpNumber", boost::program_options::value<bool>()->default_value(false)->implicit_value(true), "(flag) Should we estimate the expected number of UP vertices?")
#define HELP_OPTION ("help", "Display this message")
#define INPUT_GRAPH_OPTION GRID_GRAPH_OPTION GRAPH_FILE_OPTION TORUS_GRAPH_OPTION HEXAGONAL_GRID_OPTION

namespace discreteGermGrain
{
	bool readN(boost::program_options::variables_map& map, int& out);
	bool readGridGraph(boost::program_options::variables_map& variableMap, int& gridDimension, std::string& message);
	void readSeed(boost::program_options::variables_map& variableMap, boost::mt19937& randomSource);
	bool readInitialRadius(boost::program_options::variables_map& variableMap, int& out);
	bool readNGraphs(boost::program_options::variables_map& variableMap, int& out);
}
#endif
