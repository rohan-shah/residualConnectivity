#include <boost/accumulators/accumulators.hpp>
#include "arguments.h"
#include <limits>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/sum.hpp>
#include <boost/accumulators/statistics/sum_kahan.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
namespace discreteGermGrain
{
	int main(int argc, char** argv)
	{
		boost::program_options::options_description options("Usage");
		options.add_options()
			("nPoints", boost::program_options::value<int>(), "The number of different values strictly between 0 and 1 to use for the grid search. Values are equally spaced. ")
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
				"This program determines the minimum variance imporance sampling density using a grid search."
				"It outputs a matrix to standard output. The first row lists the true model parameter used for that column. "
				"The second row lists the best importance sampling parameter."
				"The remaining rows list the variances when using the stated importance sampling parameter. "
				"This program accepts as input data with the format of the output of ExhaustiveSearch. "
				"So it is only practical for small grids. \n\n"
			;
			std::cout << options << std::endl;
			return 0;
		}
		if(variableMap.count("nPoints") != 1)
		{
			std::cout << "Please enter exactly one value for command line argument nPoints" << std::endl;
			return 0;
		}
		int nPoints = variableMap["nPoints"].as<int>();
		std::vector<std::string> lines;
		std::string line;
		while(std::getline(std::cin, line))
		{
			lines.push_back(line);
		}
		std::size_t nLines = lines.size();
		int gridSize = (int)(sqrt(nLines - 2) + 0.5f);
		if((int)nLines != gridSize * gridSize + 2)
		{
			std::cout << "Invalid number of lines of input entered" << std::endl;
			return 0;
		}
		if(lines[0] != "Number of connected subgraphs with that number of points")
		{
			std::cout << "First line was invalid" << std::endl;
			return 0;
		}
		std::vector<int> graphCounts;
		for(std::vector<std::string>::iterator i = lines.begin()+1; i!=lines.end(); i++)
		{
			std::stringstream ss;
			ss << *i;
			int count;
			ss >> count;
			std::string colon;
			ss >> colon;
			int nGraphs;
			ss >> nGraphs;
			graphCounts.push_back(nGraphs);
		}
		//set up vector of probabilities
		std::vector<mpfr_class> probabilities;
		for(int i = 0; i < nPoints; i++)
		{
			probabilities.push_back(mpfr_class(i+1) / (nPoints+1));
		}
		std::vector<mpfr_class> connectivityProbabilities(nPoints);
		//work out percolation probabilities
		for(int i = 0; i < nPoints; i++)
		{
			mpfr_class trueProbability = probabilities[i];
			mpfr_class sum = 0;
			for(int k = 1; k <= gridSize * gridSize; k++)
			{
				mpfr_class inopProbability = 1 - trueProbability;
				mpfr_class part1 = boost::multiprecision::pow(trueProbability, k);
				mpfr_class part2 = boost::multiprecision::pow(inopProbability, gridSize * gridSize - k);
				sum += graphCounts[k] *part1*part2;
				//parts.push_back(graphCounts[k] * std::pow(trueProbability, k) * std::pow(1 - trueProbability, gridSize * gridSize - k));
			}
			connectivityProbabilities[i] = sum;
		}

		//work out variances, and use these to get the minimum variance (among a certain class of models) importance sampling densities
		std::vector<mpfr_class> bestProbability;
		//Rows of variances all use the same underlying probability model for the data. What changes is the parameter used for the importance sampling density. 
		boost::numeric::ublas::matrix<double> variances(nPoints, nPoints);
		for(int i = 0; i < nPoints; i++)
		{
			//This is the probability to use for the true density
			mpfr_class trueProbability = probabilities[i];
			mpfr_class trueInopProbability = 1 - trueProbability;
			//what is the best probability for importance sampling?
			for(int j = 0; j < nPoints; j++)
			{
				mpfr_class importanceProbability = probabilities[j];
				mpfr_class fraction = trueProbability / importanceProbability;
				mpfr_class inopFraction = (1-trueProbability) / (1-importanceProbability);
				//Work out the importance sampling variance (or something that's only a fixed constant different from it). 
				//We do this by conditioning on the number of points (k)
				mpfr_class sum = 0;
				for(int k = 1; k <= gridSize * gridSize; k++)
				{
					//ratio of densities
					mpfr_class densityRatio = boost::multiprecision::pow(fraction, k) * boost::multiprecision::pow(inopFraction, gridSize * gridSize - k);
					mpfr_class probability = graphCounts[k] * boost::multiprecision::pow(trueProbability, k) * boost::multiprecision::pow(trueInopProbability, gridSize * gridSize - k);
					mpfr_class product = densityRatio * probability;
					sum += product;
					//double densityRatio = std::pow(trueProbability / importanceProbability, k) * std::pow((1-trueProbability) / (1-importanceProbability), gridSize * gridSize - k);
					//The probability of having k points, AND that the graph is connected (under the true density, not the importance density)
					//double probability = graphCounts[k] * std::pow(trueProbability, k) * std::pow(1-trueProbability, gridSize * gridSize - k);
					//parts.push_back(densityRatio * probability);
				}

				variances(i, j) = ((mpfr_class)(sum - connectivityProbabilities[i] * connectivityProbabilities[i])).convert_to<double>();
			}
			boost::numeric::ublas::matrix_row<boost::numeric::ublas::matrix<double> > varianceRow(variances, i);
			bestProbability.push_back(probabilities[std::distance(varianceRow.begin(), std::min_element(varianceRow.begin(), varianceRow.end()))]);
		}
		//row names
		for(std::size_t i = 0; i < probabilities.size()-1; i++) std::cout << i << " ";
		std::cout << probabilities.size()-1 << std::endl;
		//output model probabilities
		std::cout << "True_Probability ";
		for(std::size_t i = 0; i < probabilities.size() - 1; i++)
		{
			std::cout << probabilities[i] << " ";
		}
		std::cout << probabilities[probabilities.size()-1] << std::endl;

		//output minimum variance probabilities
		std::cout << "Best_Importance_Probability ";
		for(std::size_t i = 0; i < bestProbability.size() - 1; i++)
		{
			std::cout << bestProbability[i] << " ";
		}
		std::cout << bestProbability[bestProbability.size()-1] << std::endl;
		//output variances of minimum variance estimator
		for(int j = 0; j < nPoints; j++)
		{
			std::cout << "Variances_Proposal_" << probabilities[j] << " ";
			for(int i = 0; i < nPoints; i++)
			{
				std::cout << variances(i, j) << " ";
			}
			std::cout << std::endl;
		}
		return 1;
	}
}
int main(int argc, char **argv)
{
	return discreteGermGrain::main(argc, argv);
}
