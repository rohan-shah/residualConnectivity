#ifndef TRANSFER_MATRIX_STATES_HEADER_GUARD
#define TRANSFER_MATRIX_STATES_HEADER_GUARD
#include <vector>
namespace residualConnectivity
{
	class transferStates
	{
	public:
		transferStates(int gridDimension);
		const std::vector<unsigned long long>& getStates() const;
		const std::vector<int>& getStateVertexCounts() const;
		int getGridDimension() const;
		/*
		   Sort the states in allStates according to their numeric values (and make corresponding changes to stateVertexCounts). This function still keeps the 0 state at the start and the -1 state at the end though. 
		   */
		void sort();
	private:
		std::vector<unsigned long long> allStates;
		std::vector<int> stateVertexCounts;
		int gridDimension;
	};
}
#endif
