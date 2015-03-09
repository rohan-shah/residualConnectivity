#include "sortStatesAndVertexCounts.h"
#include <algorithm>
#include <boost/iterator/counting_iterator.hpp>
namespace discreteGermGrain
{
	void sortStatesAndVertexCounts(std::vector<unsigned long long>& states, std::vector<int>& stateVertexCounts)
	{
		const std::size_t stateSize = states.size();
		//complicated sorting here, to keep the stateVertexCounts consistent as well
		std::vector<int> indices(boost::counting_iterator<int>(0), boost::counting_iterator<int>((int)stateSize));

		struct indicesByState
		{
		private:
			const std::vector<unsigned long long>& states;
		public:
			indicesByState(const std::vector<unsigned long long>& states)
				:states(states)
			{}
			bool operator()(int i, int j)
			{
				return states[i] < states[j];
			}
		};
		indicesByState indicesByStateSorter(states);
		std::sort(indices.begin() + 1, indices.end()-1, indicesByStateSorter);
		{
			std::vector<int> newStateVertexCounts(stateSize);
			for(std::size_t i = 0; i < states.size(); i++)
			{
				newStateVertexCounts[i] = stateVertexCounts[indices[i]];
			}
			newStateVertexCounts.swap(stateVertexCounts);
		}

		//NOW sort the states
		std::sort(states.begin()+1, states.end()-1);
	}
}
