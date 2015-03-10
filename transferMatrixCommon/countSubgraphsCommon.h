#ifndef COUNT_SUBGRAPHS_COMMON_HEADER_GUARD
#define COUNT_SUBGRAPHS_COMMON_HEADER_GUARD
#include <Eigen/Core>
#include <vector>
#include "includeNumerics.h"
#include "states.h"
namespace discreteGermGrain
{
	enum VertexState
	{
		OFF = 0, UPPER = 1, MID = 2, LOWER = 3, SINGLE = 4
	};
	typedef Eigen::Matrix<mpz_class, -1, -1, Eigen::RowMajor, -1, -1> LargeDenseIntMatrix;

	typedef std::vector<std::vector<unsigned long long> > LargeSparseIntMatrix;

	typedef Eigen::Matrix<mpz_class, 1, -1, Eigen::RowMajor, 1, -1> InitialRowVector;
	typedef Eigen::Matrix<mpz_class, -1, 1, Eigen::ColMajor, -1, 1> FinalColumnVector;

	void generateCachedIndices(unsigned long long binaryEncoding, std::vector<std::size_t>& cachedIndices, int nGroups, const transferStates& states);
	void generateBinaryMasks(const transferStates& states, std::vector<unsigned long long>& binaryMasks);
	std::string stateToString(unsigned long long state, int gridDimension);
	void generateStateGroupBinaryMasks(const transferStates& states, std::vector<std::vector<unsigned long long> >& stateGroupBinaryMasks);
}
#endif
