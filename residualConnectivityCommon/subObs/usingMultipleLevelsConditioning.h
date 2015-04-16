#ifndef DISCRETE_GERM_GRAIN_SUB_OBS_USING_MULTIPLE_LEVELS_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_SUB_OBS_USING_MULTIPLE_LEVELS_HEADER_GUARD
#include "subObs/subObs.h"
#include "Context.h"
#include "subObsTypes.h"
#include "subObs/getObservation.hpp"
#include "obs/getSubObservation.hpp"
#include "subObs/withWeight.h"
namespace discreteGermGrain
{
	namespace obs
	{
		class usingMultipleLevelsConditioning;
	}
	namespace subObs
	{
		class usingMultipleLevelsConditioning : public ::discreteGermGrain::subObs::withWeight
		{
		public:
			template<class T> friend class ::discreteGermGrain::subObs::getObservation;
			template<class T> friend class ::discreteGermGrain::obs::getSubObservation;

			typedef ::discreteGermGrain::obs::usingMultipleLevelsConditioning observationType;
			typedef ::discreteGermGrain::obs::withWeightConstructorType observationConstructorType;

			usingMultipleLevelsConditioning(usingMultipleLevelsConditioning&& other);
			void estimateRadius1(boost::mt19937& randomSource, int nSimulations, std::vector<int>& scratchMemory, boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType& stack, std::vector<observationType>& outputObservations) const;
			bool isPotentiallyConnected() const;
			usingMultipleLevelsConditioning(Context const& context, boost::shared_array<vertexState> state, int radius, ::discreteGermGrain::subObs::withWeightConstructorType &);
			void getObservation(vertexState* state, boost::mt19937& randomSource, observationConstructorType&) const;
			usingMultipleLevelsConditioning copyWithWeight(mpfr_class weight) const;
		protected:
			usingMultipleLevelsConditioning(const usingMultipleLevelsConditioning& other, mpfr_class weight);
		private:
			usingMultipleLevelsConditioning(const usingMultipleLevelsConditioning& other);
			bool potentiallyConnected;
		};
	}
}
#endif
