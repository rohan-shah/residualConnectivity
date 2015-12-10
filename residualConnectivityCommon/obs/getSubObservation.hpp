#ifndef RESIDUAL_CONNECTIVITY_OBS_GET_SUB_OBSERVATION_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_OBS_GET_SUB_OBSERVATION_HEADER_GUARD
#include "Context.h"
#include <boost/noncopyable.hpp>
namespace residualConnectivity
{
	namespace obs
	{
		template<class T> class getSubObservation : public boost::noncopyable
		{
			public:
				static typename T::subObservationType get(const T& input, int radius)
				{
					const Context& context = input.getContext();
					typename T::subObservationConstructorType otherData;
					boost::shared_array<vertexState> newState(new vertexState[context.nVertices()]);
					input.getSubObservation(newState.get(), radius, otherData);

					typename T::subObservationType retVal(context, newState, radius, otherData);
					return retVal;
				}
				template<typename U> static typename T::subObservationType get(const T& input, int radius, const U& aux)
				{
					const Context& context = input.getContext();
					typename T::subObservationConstructorType otherData(aux);
					boost::shared_array<vertexState> newState(new vertexState[context.nVertices()]);
					input.getSubObservation(newState.get(), radius, otherData);

					typename T::subObservationType retVal(context, newState, radius, otherData);
					return retVal;
				}
			private:
				getSubObservation();
		};
	}
}
#endif

