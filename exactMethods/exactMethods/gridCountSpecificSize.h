#ifndef GRID_COUNT_METHOD_HEADER_GUARD
#define GRID_COUNT_METHOD_HEADER_GUARD
#include "context.h"
namespace residualConnectivity
{
	mpz_class gridCountSpecificSizeMultiThreaded(int gridDimension, int size);
	mpz_class gridCountSpecificSizeSingleThreaded(int gridDimension, int size);
}
#endif
