#include <boost/program_options.hpp>
#include "context.h"
#include "arguments.h"
#include <boost/scoped_array.hpp>
#include "transferMatrixCommon/countSubgraphs.h"
#include "transferMatrixCommon/countSubgraphsBySize.h"
#include "transferMatrixCommon/countSubgraphsCommon.h"
#include "transferMatrixCommon/constructMatrices.h"
#include <fstream>
namespace residualConnectivity
{
	class transferMatrixLogger : public countSubgraphsBySizeLogger, public countSubgraphsLogger
	{
	public:
		virtual void beginComputeStates()
		{
			std::cout << "Computing states...." << std::endl;
		}
		virtual void endComputeStates(const transferStates& states)
		{
			std::cout << "Finished computing states, " << states.getStates().size() << " states found. " << std::endl;
		}
		virtual void beginConstructDenseMatrices()
		{
			std::cout << "Beginning construction of transition matrix..." << std::endl;
		}
		virtual void endConstructDenseMatrices(LargeDenseIntMatrix& transitionMatrix, FinalColumnVector& final, InitialRowVector& initial, std::size_t nonZeroTransitionCount)
		{
			std::cout << "Finished constructing transition matrix, " << nonZeroTransitionCount << " / " << transitionMatrix.innerSize()*transitionMatrix.innerSize() << " transitions are possible." << std::endl;
		}
		virtual void completedMultiplicationStep(int completed, int total)
		{
			std::cout << "Completed multiplication step " << completed << " / " << total << "." <<std::endl;
		}
		virtual void endMultiplications()
		{
			std::cout << "Finished multiplication steps." << std::endl;
		}
		virtual void beginCheckFinalStates()
		{
			std::cout << "Checking valid final states." << std::endl;
		}
		virtual void endCheckFinalStates()
		{
			std::cout << "Finished checking valid final states. " << std::endl;
		}
	};
	int main(int argc, char **argv)
	{
		boost::program_options::options_description options("Usage");
		options.add_options()
			GRID_GRAPH_OPTION
			("bySize", boost::program_options::bool_switch()->default_value(false), "(flag) Should we attempt to also count the number of vertices for each subgraph?")
			("outputFile", boost::program_options::value<std::string>(), "(string) File to output count data to")
			("multithreaded", boost::program_options::value<bool>()->default_value(true)->implicit_value(true), "(flag) Should we use multithreading?")
			HELP_OPTION;

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
		bool multithreaded = variableMap["multithreaded"].as<bool>();
		if(!bySize)
		{
			if(variableMap.count("outputFile") > 0)
			{
				std::cout << "Input `outputFile' only valid with option --bySize" << std::endl;
				return 0;
			}
			std::size_t nonZeroCount;
			LargeDenseIntMatrix transitionMatrix;
			transferMatrixLogger logger;
			mpz_class result = countSubgraphsMultiThreaded(gridDimension, transitionMatrix, nonZeroCount, &logger);
			std::cout << "Result was " << result.str() << std::endl;
		}
		else
		{
			if(variableMap.count("outputFile") == 0)
			{
				std::cout << "Input `outputFile' is required with option --bySize" << std::endl;
				return 0;
			}
			std::string outputFile = variableMap["outputFile"].as<std::string>();
			std::ofstream outputStream(outputFile.c_str(), std::ios_base::out);
			if(!outputStream)
			{
				std::cout << "Unable to open specified file for writing" << std::endl;
				return 0;
			}
			boost::scoped_array<mpz_class> counts(new mpz_class[gridDimension*gridDimension+1]);
			std::size_t nonZeroCount = 0;
			TransitionMatrix transitionMatrix;
			transferMatrixLogger logger;
#ifdef USE_OPENMP
			if(multithreaded) countSubgraphsBySizeMultiThreaded(counts.get(), gridDimension, transitionMatrix, nonZeroCount, &logger);
			else countSubgraphsBySizeSingleThreaded(counts.get(), gridDimension, transitionMatrix, nonZeroCount, &logger);
#else
			countSubgraphsBySizeSingleThreaded(counts.get(), gridDimension, transitionMatrix, nonZeroCount, &logger);
#endif

			outputStream << "Number of connected subgraphs with that number of points" << std::endl;
			for(int i = 0; i < gridDimension*gridDimension+1; i++)
			{
				outputStream << std::setw(3) << i << ":  " << counts[i].str() << std::endl;
			}
			outputStream.flush();
			outputStream.close();
		}
		return 0;
	}
}
int main(int argc, char **argv)
{
	return residualConnectivity::main(argc, argv);
}
