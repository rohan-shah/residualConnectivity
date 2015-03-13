#include "DiscreteGermGrainObs.h"
#include "DiscreteGermGrainSubObs.h"
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
	DiscreteGermGrainObs::DiscreteGermGrainObs(Context const& context, boost::archive::binary_iarchive& archive)
		:context(context)
	{
		archive >> *this;
	}
	DiscreteGermGrainObs::DiscreteGermGrainObs(Context const& context, boost::archive::text_iarchive& archive)
		:context(context)
	{
		archive >> *this;
	}
	DiscreteGermGrainObs::DiscreteGermGrainObs(Context const& context, boost::mt19937& randomSource)
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
	DiscreteGermGrainObs::DiscreteGermGrainObs(Context const& context, boost::shared_array<const vertexState> state, ::discreteGermGrain::obs::basicConstructorType&)
		:context(context), state(state)
	{
	}
	DiscreteGermGrainObs::DiscreteGermGrainObs(Context const& context, boost::shared_array<const vertexState> state)
		: context(context), state(state)
	{
	}
	DiscreteGermGrainObs::DiscreteGermGrainObs(DiscreteGermGrainObs&& other)
		:context(other.context)
	{
		state.swap(other.state);
	}
	Context const& DiscreteGermGrainObs::getContext() const
	{
		return context;
	}
	DiscreteGermGrainObs& DiscreteGermGrainObs::operator=(DiscreteGermGrainObs&& other)
	{
		if(&context != &(other.context)) throw std::runtime_error("Attempting to mix different Contexts!");
		state.swap(other.state);
		return *this;
	}
	const vertexState* DiscreteGermGrainObs::getState() const
	{
		return state.get();
	}
}
