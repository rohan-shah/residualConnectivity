#include <boost/program_options.hpp>
#include "Context.h"
#include "arguments.h"
#include <boost/scoped_array.hpp>
#include "countSubgraphs.h"
#include "countSubgraphsBySize.h"
#include "countSubgraphsCommon.h"
#include "constructMatrices.h"
#include <fstream>
namespace discreteGermGrain
{
	int main(int argc, char **argv)
	{
		boost::program_options::options_description options("Usage");
		options.add_options()
			("gridGraph", boost::program_options::value<int>(), "(int) The dimension of the square grid graph to use. ")
			("bySize", boost::program_options::bool_switch()->default_value(false), "(flag) Should we attempt to also count the number of vertices for each subgraph?")
			("saveMatrix", boost::program_options::bool_switch()->default_value(false), "(flag) Should we save the transition matrix?")
			("diagnostics", boost::program_options::bool_switch()->default_value(false), "(flag) Should we output diagnostic information?")
			("help", "Display this message");

		boost::program_options::variables_map variableMap;
		try
		{
			boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options), variableMap);
		}
		catch(boost::program_options::error& ee)
		{
			std::cerr << "Error parsing command line arguments: " << ee.what() << std::endl << std::endl;
			std::cerr << options << std::endl;
			return -1;
		}
		if(variableMap.count("help") > 0)
		{
			std::cout <<
				"This uses a transfer matrix approach to compute the connectivity probability. It can also count the total number of connected subgraphs."
				"\n\n"
			;
			std::cout << options << std::endl;
			return 0;
		}

		int gridDimension;
		std::string message;
		if(!readGridGraph(variableMap, gridDimension, message))
		{
			std::cout << message << std::endl;
			return 0;
		}
		bool bySize = variableMap["bySize"].as<bool>();
		bool saveMatrix = variableMap["saveMatrix"].as<bool>();
		bool diagnostics = variableMap["diagnostics"].as<bool>();
		if(!bySize)
		{
			std::size_t nonZeroCount;
			LargeDenseIntMatrix transitionMatrix;
			mpz_class result = countSubgraphsMultiThreaded(gridDimension, transitionMatrix, nonZeroCount);

			if(diagnostics)
			{
				std::size_t stateSize = transitionMatrix.innerSize();
				std::cout << "Used " << stateSize << " states, with " << 100*((float)nonZeroCount / (float)(stateSize * stateSize)) << "% nonzero entries" << std::endl;
			}
			char* resultCStr = mpz_get_str(NULL, 10, result.get_mpz_t());
			std::string resultStr = resultCStr;

			std::cout << "Result was " << resultCStr << std::endl;
			free(resultCStr);

			if(saveMatrix) 
			{
				Eigen::Matrix<unsigned long long, -1, -1, Eigen::RowMajor, -1, -1> smallerTransitionMatrix;
				for(int i = 0; i < transitionMatrix.innerSize(); i++)
				{
					for(int j = 0; j < transitionMatrix.innerSize(); j++)
					{
						smallerTransitionMatrix(i, j) = transitionMatrix(i, j).get_si();
					}
				}

				std::ofstream ofs("./matrix.out");
				ofs << smallerTransitionMatrix;
				ofs.flush();
				ofs.close();
			}
		}
		else
		{
			boost::scoped_array<mpz_class> counts(new mpz_class[gridDimension*gridDimension+1]);
			std::size_t nonZeroCount = 0;
			LargeDenseIntMatrix transitionMatrix;

			countSubgraphsBySizeMultiThreaded(counts.get(), gridDimension, transitionMatrix, nonZeroCount);

			if(diagnostics)
			{
				std::size_t stateSize = transitionMatrix.innerSize();
				std::cerr << "Used " << stateSize << " states, with " << 100*((float)nonZeroCount / (float)(stateSize * stateSize)) << "% nonzero entries" << std::endl;
			}
			std::cout << "Number of connected subgraphs with that number of points" << std::endl;
			for(int i = 0; i < gridDimension*gridDimension+1; i++)
			{
				char* resultCStr = mpz_get_str(NULL, 10, counts[i].get_mpz_t());
				std::string resultStr = resultCStr;
				std::cout << std::setw(3) << i << ":  " << resultStr << std::endl;
				free(resultCStr);
			}
		}
		return 0;
	}
}
int main(int argc, char **argv)
{
	return discreteGermGrain::main(argc, argv);
}
