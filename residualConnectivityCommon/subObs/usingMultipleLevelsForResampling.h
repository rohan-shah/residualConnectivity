#ifndef DISCRETE_GERM_GRAIN_SUB_OBS_USING_MULTIPLE_LEVELS_FOR_RESAMPLING_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_SUB_OBS_USING_MULTIPLE_LEVELS_FOR_RESAMPLING_HEADER_GUARD
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
		class usingMultipleLevelsForResampling;
	}
	namespace subObs
	{
		class usingMultipleLevelsForResampling : public ::discreteGermGrain::subObs::withWeight
		{
		public:
			template<class T> friend class ::discreteGermGrain::subObs::getObservation;
			template<class T> friend class ::discreteGermGrain::obs::getSubObservation;

			typedef ::discreteGermGrain::obs::usingMultipleLevelsForResampling observationType;
			typedef ::discreteGermGrain::obs::withWeightConstructorType observationConstructorType;

			usingMultipleLevelsForResampling(usingMultipleLevelsForResampling&& other);
			bool isPotentiallyConnected() const;
			usingMultipleLevelsForResampling(Context const& context, boost::shared_array<vertexState> state, int radius, ::discreteGermGrain::subObs::usingMultipleLevelsForResamplingConstructorType &);
			void getObservation(vertexState* state, boost::mt19937& randomSource, observationConstructorType&) const;
			usingMultipleLevelsForResampling copyWithWeight(mpfr_class weight) const;
		protected:
			usingMultipleLevelsForResampling(const usingMultipleLevelsForResampling& other, mpfr_class weight);
		private:
			usingMultipleLevelsForResampling(const usingMultipleLevelsForResampling& other);
			bool potentiallyConnected;
		};
	}
}
#endif
