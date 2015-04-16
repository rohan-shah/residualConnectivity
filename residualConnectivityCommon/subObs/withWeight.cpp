#include "subObs/withWeight.h"
namespace discreteGermGrain
{
	namespace subObs
	{
		withWeight::withWeight(Context const& context, boost::shared_array<const vertexState> state, int radius, mpfr_class weight)
			: ::discreteGermGrain::subObs::subObsWithRadius(context, state, radius), weight(weight)
		{}
		withWeight::withWeight(withWeight&& other)
			: ::discreteGermGrain::subObs::subObsWithRadius(static_cast<withWeight&&>(other)), weight(other.weight)
		{}
		const mpfr_class& withWeight::getWeight() const
		{
			return weight;
		}
		withWeight::withWeight(const withWeight& other)
			: ::discreteGermGrain::subObs::subObsWithRadius(static_cast<const ::discreteGermGrain::subObs::subObsWithRadius&>(other)), weight(other.weight)
		{}
		withWeight::withWeight(const withWeight& other, mpfr_class newWeight)
			: ::discreteGermGrain::subObs::subObsWithRadius(static_cast<const ::discreteGermGrain::subObs::subObsWithRadius&>(other)), weight(newWeight)
		{}

	}
}
