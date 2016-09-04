#include <boost/archive/basic_archive.hpp>
#include "aliasMethod.h"
#include <algorithm>
#include "empiricalDistribution.h"
#include "observationTree.h"
#include <boost/random/mersenne_twister.hpp>
#include "observation.h"
#include "obs/articulationConditioningSameCount.h"
#include "subObs/articulationConditioningSameCount.h"
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include "bridges.hpp"
#include "obs/getSubObservation.hpp"
#ifdef USE_OPENMP
	#include <omp.h>
#endif
#include "isSingleComponentWithRadius.h"
#include "monteCarloMethods/articulationConditioningSameCount.h"
namespace residualConnectivity
{
	void articulationConditioningSameCount(articulationConditioningSameCountArgs& args)
	{
		const std::size_t nVertices = args.contextObj.nVertices();
	}
}

