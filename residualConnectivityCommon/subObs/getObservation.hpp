#ifndef DISCRETE_GERM_GRAIN_OBS_GET_SUBOBSERVATION_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_OBS_GET_SUBOBSERVATION_HEADER_GUARD
#include <boost/random/mersenne_twister.hpp>
#include "Context.h"
#include <boost/shared_array.hpp>
#include <boost/noncopyable.hpp>
namespace discreteGermGrain
{
	namespace subObs
	{
		template<class T> class getObservation : public boost::noncopyable
		{
			public:
				static typename T::observationType get(const T& input, boost::random::mt19937& randomSource)
				{
					const Context& context = input.getContext();
					std::size_t nVertices = context.nVertices();
					boost::shared_array<vertexState> state(new vertexState[nVertices]);
					typename T::observationConstructorType otherData;
					input.getObservation(state.get(), randomSource, otherData);
					typename T::observationType returnVal(input.getContext(), state, otherData);
					return returnVal;
				}
				template<typename U> static typename T::observationType get(const T& input, boost::random::mt19937& randomSource, U& aux)
				{
					const Context& context = input.getContext();
					std::size_t nVertices = context.nVertices();
					boost::shared_array<vertexState> state(new vertexState[nVertices]);
					typename T::observationConstructorType otherData(aux);
					input.getObservation(state.get(), randomSource, otherData);
					typename T::observationType returnVal(input.getContext(), state, otherData);
					return returnVal;
				}
			private:
				getObservation();
		};
	}
}
#endif
