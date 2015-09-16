#include <boost/program_options.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/sum_kahan.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include "arguments.h"
#include "constructMatrices.h"
#include "states.h"
#include <boost/scoped_array.hpp>
#include "includeNumerics.h"
#include "argumentsMPFR.h"
#include <boost/noncopyable.hpp>
namespace residualConnectivity
{
	struct TransferEstimationData : public boost::noncopyable
	{
	private:
		struct sorter
		{
			sorter(const std::vector<int>& vertexCounts)
				:vertexCounts(vertexCounts)
			{};
			bool operator()(const int first, const int second)
			{
				return vertexCounts[first] < vertexCounts[second];
			};
			const std::vector<int>& vertexCounts;
		};
	public:
		TransferEstimationData(TransferEstimationData&& other)
		{
			possibleDestinations.swap(other.possibleDestinations);
			destinationVertexCountRanges.swap(other.destinationVertexCountRanges);
			destinationVertexCountCumulativeProbabilities.swap(other.destinationVertexCountCumulativeProbabilities);
			std::swap(connectivityProbability, other.connectivityProbability);
		}
		TransferEstimationData& operator=(TransferEstimationData&& other)
		{
			possibleDestinations.swap(other.possibleDestinations);
			destinationVertexCountRanges.swap(other.destinationVertexCountRanges);
			destinationVertexCountCumulativeProbabilities.swap(other.destinationVertexCountCumulativeProbabilities);
			std::swap(connectivityProbability, other.connectivityProbability);
			return *this;
		}
		TransferEstimationData(const std::vector<unsigned long long>& unsortedPossibleDestinations, const std::vector<int>& destinationVertexCounts, std::vector<int>& scratch, mpfr_class probability, int gridDimension)
		{
			const std::size_t nDestinations = unsortedPossibleDestinations.size();

			scratch.resize(nDestinations);
			//Fill the scratch data with the indices of the possibleDestinations data
			for(std::size_t i = 0; i < nDestinations; i++) scratch[i] = (int)i;

			//sort the indices by the destinationVertexCount data.
			sorter indexSorter(destinationVertexCounts);
			std::sort(scratch.begin(), scratch.end(), indexSorter);

			//Construct the possibleDestination data, sorted by the vertex count data.
			possibleDestinations.resize(nDestinations);
			//what's the first index with this vertex count?
			std::size_t previousStart = 0;
			
			possibleDestinations[0] = unsortedPossibleDestinations[scratch[0]];
			int extraCount = destinationVertexCounts[scratch[0]];
			while(extraCount--)
			{
				destinationVertexCountRanges.push_back(std::make_pair(-1, -1));
			}
			for(std::size_t i = 1; i < nDestinations; i++)
			{
				//sort the possibel destination data
				possibleDestinations[i] = unsortedPossibleDestinations[scratch[i]];
				//are we at a transition where the vertex count increases?
				if(destinationVertexCounts[scratch[i]] != destinationVertexCounts[scratch[i-1]])
				{
					destinationVertexCountRanges.push_back(std::make_pair(previousStart, (int)i-1));
					//if it increases by more than one then we need to put in extra data
					int extraCount = destinationVertexCounts[scratch[i]] - (destinationVertexCounts[scratch[i-1]] + 1);
					while(extraCount--)
					{
						destinationVertexCountRanges.push_back(std::make_pair(-1, -1));
					}
					previousStart = i;
				}
			}
			destinationVertexCountRanges.push_back(std::make_pair(previousStart, nDestinations-1));

			std::vector<std::pair<std::size_t, std::size_t> >::iterator currentRange = destinationVertexCountRanges.begin();
			//A temporary vertex for the unnormalised vertex count probabilities
			std::vector<double> unnormalisedCountProbabilities;
			boost::accumulators::accumulator_set<double, boost::accumulators::stats<boost::accumulators::tag::sum_kahan> > acc;
			
			int currentVertexCount = 0;
			while(currentRange != destinationVertexCountRanges.end())
			{
				if(currentRange->first == (unsigned long long)-1 && currentRange->second == (unsigned long long)-1)
				{
					unnormalisedCountProbabilities.push_back(0);
					currentVertexCount++;
					currentRange++;
				}
				else
				{
					//number of transitions to states with this number of vertices;
					std::size_t nTransitions = currentRange->second - currentRange->first + 1;
					//probabilities of going to a state with this number of vertices
					double currentCountProbability = nTransitions * pow(probability.convert_to<double>(), currentVertexCount) * pow(1-probability.convert_to<double>(), gridDimension - currentVertexCount);
					unnormalisedCountProbabilities.push_back(currentCountProbability);
					acc(currentCountProbability);
					currentRange++;
					currentVertexCount++;
				}
			}

			connectivityProbability = boost::accumulators::sum_kahan(acc);
			//construct cumulative data
			double currentLevel = 0;
			for(std::vector<double>::iterator i = unnormalisedCountProbabilities.begin(); i != unnormalisedCountProbabilities.end(); i++)
			{
				currentLevel += *i;
				destinationVertexCountCumulativeProbabilities.push_back(currentLevel / connectivityProbability);
			}
			//make sure the largest value is a 1
			destinationVertexCountCumulativeProbabilities.pop_back();
			destinationVertexCountCumulativeProbabilities.push_back(1);
		};
		unsigned long long sample(boost::mt19937& randomSource)
		{
			boost::random::uniform_01<> uniformDistribution;
			double numberOfVerticesRandom = uniformDistribution(randomSource);
			int nVertices = 0;
			while(numberOfVerticesRandom > destinationVertexCountCumulativeProbabilities[nVertices])
			{
				nVertices++;
			}

			std::pair<std::size_t, std::size_t> range = destinationVertexCountRanges[nVertices];
			boost::random::uniform_int_distribution<std::size_t> stateIndexDistribution(range.first, range.second);
			std::size_t index = stateIndexDistribution(randomSource);
			return possibleDestinations[index];
		}
		std::vector<unsigned long long> possibleDestinations;
		std::vector<std::pair<std::size_t, std::size_t> > destinationVertexCountRanges;
		std::vector<double> destinationVertexCountCumulativeProbabilities;

		double connectivityProbability;
	};
	int main(int argc, char **argv)
	{
		boost::program_options::options_description options("Usage");
		options.add_options()
			GRID_GRAPH_OPTION
			PROBABILITY_OPTION
			N_OPTION
			SEED_OPTION
			("diagnostics", boost::program_options::bool_switch()->default_value(false), "(flag) Should we output diagnostic information?")
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

		mpfr_class probability;
		if(!readProbabilityString(variableMap, probability, message))
		{
			std::cout << message << std::endl;
			return 0;
		}
		int n;
		if(!readN(variableMap, n))
		{
			std::cout << "Invalid value entered for input `n'" << std::endl;
			return 0;
		}
		boost::mt19937 randomSource;
		readSeed(variableMap, randomSource);

		bool diagnostics = variableMap["diagnostics"].as<bool>();

		transferStates states(gridDimension);
		states.sort();
		const std::vector<unsigned long long>& allStates = states.getStates();
		const std::vector<int>& stateVertexCounts = states.getStateVertexCounts();
		const std::size_t stateSize = allStates.size();

		
		LargeSparseIntMatrix transitionMatrix;
		InitialRowVector initial;
		FinalColumnVector final;
		std::size_t nonZeroCount = 0;
		constructMatricesSparse(transitionMatrix, final, initial, states, nonZeroCount);

		if(diagnostics)
		{
			std::cerr << "Used " << stateSize << " states, with " << 100*((float)nonZeroCount / (float)(stateSize * stateSize)) << "% nonzero entries" << std::endl;
		}

		//as part of the simulation algorithm we need to be able to draw from a random starting point. 
		//There are going to be lots of these so doing the naive thing and just using the distribution is going to be inefficient.
		//Instead we simulate the NUMBER of points in the starting distribution, and then draw uniformly from the initial states
		//with that number of points. 
		std::vector<int> scratch;
		std::vector<unsigned long long> startingStates;
		std::vector<int> startingStateVertexCounts;
		for(std::size_t i = 0; i < stateSize; i++)
		{
			if(initial(i)) 
			{
				startingStates.push_back(i);
				startingStateVertexCounts.push_back(stateVertexCounts[i]);
			}
		}
		TransferEstimationData initialEstimationData(startingStates, startingStateVertexCounts, scratch, probability, gridDimension);
		//ok, now we have to do the same thing for every possible transition too. 
		std::vector<TransferEstimationData> estimationData;
		std::vector<int> destinationStateVertexCounts;
		for(std::size_t i = 0; i < stateSize; i++)
		{
			const std::vector<unsigned long long>& destinationStates = transitionMatrix[i];
			destinationStateVertexCounts.clear();
			for(std::size_t j = 0; j < destinationStates.size(); j++)
			{
				destinationStateVertexCounts.push_back(stateVertexCounts[destinationStates[j]]);
			}
			estimationData.push_back(TransferEstimationData(destinationStates, destinationStateVertexCounts, scratch, probability, gridDimension));
		}
		//Now the same thing for the final transitions.
		std::vector<TransferEstimationData> finalEstimationData;
		std::vector<unsigned long long> destinationStates;
		for(std::size_t i = 0; i < stateSize; i++)
		{
			//all possible transitions, ignoring the fact that many are not allowed in the terminal stage.
			const std::vector<unsigned long long>& allPossibleDestinations = transitionMatrix[i];
			destinationStates.clear();
			destinationStateVertexCounts.clear();
			for(std::size_t j = 0; j < allPossibleDestinations.size(); j++)
			{
				if(final(allPossibleDestinations[j])) 
				{
					destinationStateVertexCounts.push_back(stateVertexCounts[allPossibleDestinations[j]]);
					destinationStates.push_back(allPossibleDestinations[j]);
				}
			}
			finalEstimationData.push_back(TransferEstimationData(destinationStates, destinationStateVertexCounts, scratch, probability, gridDimension));
		}

		//now for the estimation
		std::vector<double> probabilities;
		for(int i = 0; i < n; i++)
		{
			unsigned long long currentStateIndex = initialEstimationData.sample(randomSource);
			double currentProbability = initialEstimationData.connectivityProbability;
			for(int j = 0; j < gridDimension-2; j++)
			{
				currentProbability *= estimationData[currentStateIndex].connectivityProbability;
				currentStateIndex = estimationData[currentStateIndex].sample(randomSource);
			}
			currentProbability *= finalEstimationData[currentStateIndex].connectivityProbability;
			probabilities.push_back(currentProbability);
		}
		boost::accumulators::accumulator_set<double, boost::accumulators::stats<boost::accumulators::tag::sum_kahan> > acc;
		std::for_each(probabilities.begin(), probabilities.end(), std::ref(acc));
		double sum = boost::accumulators::sum_kahan(acc);
		double mean = sum / n;

		std::cout << "Estimate was " << mean << std::endl;

		return 0;
	}
}
int main(int argc, char **argv)
{
	return residualConnectivity::main(argc, argv);
}
