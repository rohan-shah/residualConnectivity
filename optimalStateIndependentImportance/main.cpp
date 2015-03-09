#include <boost/accumulators/accumulators.hpp>
#include "arguments.h"
#include <limits>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/sum.hpp>
#include <boost/accumulators/statistics/sum_kahan.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <mpir.h>
#include <mpirxx.h>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
namespace discreteGermGrain
{
	int main(int argc, char** argv)
	{
		//use high precision for floating point stuff
		mpf_set_default_prec(256);

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
		std::vector<float> probabilities;
		for(int i = 0; i < nPoints; i++)
		{
			probabilities.push_back(((float)i+1) / (nPoints+1));
		}
		std::vector<double> percolationProbabilities(nPoints);
		//work out percolation probabilities
		for(int i = 0; i < nPoints; i++)
		{
			float trueProbability = probabilities[i];
			std::vector<mpf_class> parts;
			for(int k = 1; k <= gridSize * gridSize; k++)
			{
				mpf_class tmpProbability = trueProbability;
				mpf_class oneMinusProbability = 1 - trueProbability;
				mpf_class part1, part2;
				mpf_pow_ui(part1.get_mpf_t(), tmpProbability.get_mpf_t(), k);
				mpf_pow_ui(part2.get_mpf_t(), oneMinusProbability.get_mpf_t(), gridSize * gridSize - k);
				parts.push_back(graphCounts[k] *part1*part2);
				//parts.push_back(graphCounts[k] * std::pow(trueProbability, k) * std::pow(1 - trueProbability, gridSize * gridSize - k));
			}
			boost::accumulators::accumulator_set<mpf_class, boost::accumulators::stats<boost::accumulators::tag::sum> > acc;
			std::for_each(parts.begin(), parts.end(), std::ref(acc));
			percolationProbabilities[i] = boost::accumulators::sum(acc).get_d();
		}

		//work out variances, and use these to get the minimum variance (among a certain class of models) importance sampling densities
		std::vector<float> bestProbability;
		//Rows of variances all use the same underlying probability model for the data. What changes is the parameter used for the importance sampling density. 
		boost::numeric::ublas::matrix<double> variances(nPoints, nPoints);
		for(int i = 0; i < nPoints; i++)
		{
			//This is the probability to use for the true density
			float trueProbability = probabilities[i];
			//what is the best probability for importance sampling?
			for(int j = 0; j < nPoints; j++)
			{
				float importanceProbability = probabilities[j];
				//Work out the importance sampling variance (or something that's only a fixed constant different from it). 
				//We do this by conditioning on the number of points (k)
				std::vector<mpf_class> parts;
				for(int k = 1; k <= gridSize * gridSize; k++)
				{
					//ratio of densities
					mpf_class densityRatio;
					{
						mpf_class part1, part2;
						mpf_class fraction = trueProbability / importanceProbability;
						mpf_pow_ui(part1.get_mpf_t(), fraction.get_mpf_t(), k);
						
						fraction = (1-trueProbability) / (1-importanceProbability);
						mpf_pow_ui(part2.get_mpf_t(), fraction.get_mpf_t(), gridSize * gridSize - k);
						densityRatio = part1 * part2;
					}
					mpf_class probability;
					{
						mpf_class part2, part3;
						mpf_class tmpProb;
						
						tmpProb = trueProbability;
						mpf_pow_ui(part2.get_mpf_t(), tmpProb.get_mpf_t(), k);
						
						tmpProb = 1 - trueProbability;
						mpf_pow_ui(part3.get_mpf_t(), tmpProb.get_mpf_t(), gridSize * gridSize - k);
						probability = graphCounts[k] * part2 * part3;
					}
					mpf_class product = densityRatio * probability;
					parts.push_back(product.get_d());
					//double densityRatio = std::pow(trueProbability / importanceProbability, k) * std::pow((1-trueProbability) / (1-importanceProbability), gridSize * gridSize - k);
					//The probability of having k points, AND that the graph is connected (under the true density, not the importance density)
					//double probability = graphCounts[k] * std::pow(trueProbability, k) * std::pow(1-trueProbability, gridSize * gridSize - k);
					//parts.push_back(densityRatio * probability);
				}

				boost::accumulators::accumulator_set<mpf_class, boost::accumulators::stats<boost::accumulators::tag::sum> > acc;
				std::for_each(parts.begin(), parts.end(), std::ref(acc));
				variances(i, j) = boost::accumulators::sum(acc).get_d() - percolationProbabilities[i] * percolationProbabilities[i];
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
