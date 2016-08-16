#include "exactMethods/optimalStateIndependentImportance.h"
namespace residualConnectivity
{
	void optimalStateIndependentImportance(std::vector<mpz_class>& graphCounts, std::vector<mpfr_class>& probabilities, std::vector<mpfr_class>& bestImportanceProbabilities)
	{
		int nVertices = (int)graphCounts.size()-1;
		int nPoints = (int)probabilities.size();
		std::vector<mpfr_class> connectivityProbabilities(nPoints);
		//work out percolation probabilities
		for(int i = 0; i < nPoints; i++)
		{
			mpfr_class trueProbability = probabilities[i];
			mpfr_class sum = 0;
			for(int k = 1; k <= nVertices; k++)
			{
				mpfr_class inopProbability = 1 - trueProbability;
				mpfr_class part1 = boost::multiprecision::pow(trueProbability, k);
				mpfr_class part2 = boost::multiprecision::pow(inopProbability, nVertices - k);
				sum += graphCounts[k] *part1*part2;
			}
			connectivityProbabilities[i] = sum;
		}

		//Rows of variances all use the same underlying probability model for the data. What changes is the parameter used for the importance sampling density. 
		bestImportanceProbabilities.clear();
		std::vector<double> variances(nPoints);
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
				for(int k = 1; k <= nVertices; k++)
				{
					//ratio of densities
					mpfr_class densityRatio = boost::multiprecision::pow(fraction, k) * boost::multiprecision::pow(inopFraction, nVertices - k);
					mpfr_class probability = graphCounts[k] * boost::multiprecision::pow(trueProbability, k) * boost::multiprecision::pow(trueInopProbability, nVertices - k);
					mpfr_class product = densityRatio * probability;
					sum += product;
				}

				variances[j] = ((mpfr_class)(sum - connectivityProbabilities[i] * connectivityProbabilities[i])).convert_to<double>();
			}
			bestImportanceProbabilities.push_back(probabilities[std::distance(variances.begin(), std::min_element(variances.begin(),variances.end()))]);
		}
	}
}
