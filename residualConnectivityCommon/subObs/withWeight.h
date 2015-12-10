#ifndef RESIDUAL_CONNECTIVITY_SUB_OBS_WITH_WEIGHT_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_SUB_OBS_WITH_WEIGHT_HEADER_GUARD
#include "subObs/subObs.h"
#include "Context.h"
#include "subObsTypes.h"
#include "subObs/getObservation.hpp"
#include "obs/getSubObservation.hpp"
namespace residualConnectivity
{
	namespace subObs
	{
		class withWeight : public ::residualConnectivity::subObs::subObsWithRadius
		{
		public:
			withWeight(withWeight&& other);
			withWeight(Context const& context, boost::shared_array<const vertexState> state, int radius, mpfr_class weight);
			const mpfr_class& getWeight() const;
		protected:
			withWeight(const withWeight& other);
			withWeight(const withWeight& other, mpfr_class weight);
			mpfr_class weight;
		};
	}
}
#endif
