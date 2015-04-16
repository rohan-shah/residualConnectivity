#include "observation.h"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/graph/connected_components.hpp>
#include <vector>
#include "lexographicOrder.h"
#include <boost/graph/copy.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
namespace discreteGermGrain
{
	observation::observation(Context const& context, boost::archive::binary_iarchive& archive)
		:context(context)
	{
		archive >> *this;
	}
	observation::observation(Context const& context, boost::archive::text_iarchive& archive)
		:context(context)
	{
		archive >> *this;
	}
	observation::observation(Context const& context, boost::mt19937& randomSource)
		: context(context), state(NULL)
	{
		std::size_t nVertices = context.nVertices();

		boost::shared_array<vertexState> state(new vertexState[nVertices]);
		this->state = state;

		double openProbability = context.getOperationalProbabilityD();
		boost::random::bernoulli_distribution<float> vertexDistribution(openProbability);
		
		//Get out which vertices are present, and the count of the total number of present vertices
		for(std::size_t i = 0; i < nVertices; i++)
		{
			if(vertexDistribution(randomSource))
			{
				state[i].state = UNFIXED_ON;
			}
			else state[i].state = UNFIXED_OFF;
		}
	}
	observation::observation(Context const& context, boost::shared_array<const vertexState> state, ::discreteGermGrain::obs::basicConstructorType&)
		:context(context), state(state)
	{
	}
	observation::observation(const observation& other)
		: context(other.context), state(other.state)
	{}
	observation::observation(Context const& context, boost::shared_array<const vertexState> state)
		: context(context), state(state)
	{
	}
	observation::observation(observation&& other)
		:context(other.context)
	{
		state.swap(other.state);
	}
	Context const& observation::getContext() const
	{
		return context;
	}
	observation& observation::operator=(observation&& other)
	{
		if(&context != &(other.context)) throw std::runtime_error("Attempting to mix different Contexts!");
		state.swap(other.state);
		return *this;
	}
	const vertexState* observation::getState() const
	{
		return state.get();
	}
	const observation& observationWithContext::getObs() const
	{
		return *obs.get();
	}
	const Context& observationWithContext::getContext() const
	{
		return *context.get();
	}
}
