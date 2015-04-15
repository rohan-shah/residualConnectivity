#ifndef DISCRETE_GERM_GRAIN_SUB_OBS_WITH_WEIGHT_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_SUB_OBS_WITH_WEIGHT_HEADER_GUARD
#include "subObs/subObs.h"
#include "Context.h"
#include "subObsTypes.h"
#include "subObs/getObservation.hpp"
#include "obs/getSubObservation.hpp"
namespace discreteGermGrain
{
	namespace subObs
	{
		class withWeight : public ::discreteGermGrain::subObs::subObsWithRadius
		{
		public:
			withWeight(withWeight&& other);
			withWeight(Context const& context, boost::shared_array<const vertexState> state, int radius, mpfr_class weight);
			const mpfr_class& getWeight() const;
		protected:
			mpfr_class weight;
		};
	}
}
#endif
