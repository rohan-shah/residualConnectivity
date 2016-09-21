#include "subObs/withWeight.h"
namespace residualConnectivity
{
	namespace subObs
	{
		withWeight::withWeight(context const& contextObj, boost::shared_array<const vertexState> state, int radius, mpfr_class weight)
			: ::residualConnectivity::subObs::subObsWithRadius(contextObj, state, radius), weight(weight)
		{}
		withWeight::withWeight(withWeight&& other)
			: ::residualConnectivity::subObs::subObsWithRadius(static_cast<withWeight&&>(other)), weight(other.weight)
		{}
		const mpfr_class& withWeight::getWeight() const
		{
			return weight;
		}
		withWeight::withWeight(const withWeight& other)
			: ::residualConnectivity::subObs::subObsWithRadius(static_cast<const ::residualConnectivity::subObs::subObsWithRadius&>(other)), weight(other.weight)
		{}
		withWeight& withWeight::operator=(const withWeight& other)
		{
			::residualConnectivity::subObs::subObsWithRadius::operator=(other);
			weight = other.weight;
			return *this;
		}
		withWeight::withWeight(const withWeight& other, mpfr_class newWeight)
			: ::residualConnectivity::subObs::subObsWithRadius(static_cast<const ::residualConnectivity::subObs::subObsWithRadius&>(other)), weight(newWeight)
		{}

	}
}
