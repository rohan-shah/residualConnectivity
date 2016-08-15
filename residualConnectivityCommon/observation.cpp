#include "observation.h"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/graph/connected_components.hpp>
#include <vector>
#include "lexographicOrder.h"
#include <boost/graph/copy.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
namespace residualConnectivity
{
	observation::observation(context const& contextObj, boost::archive::binary_iarchive& archive)
		:contextObj(contextObj)
	{
		archive >> *this;
	}
	observation::observation(context const& contextObj, boost::archive::text_iarchive& archive)
		:contextObj(contextObj)
	{
		archive >> *this;
	}
	observation::observation(context const& contextObj, boost::mt19937& randomSource)
		: contextObj(contextObj), state((vertexState*)NULL)
{
		std::size_t nVertices = contextObj.nVertices();

		boost::shared_array<vertexState> state(new vertexState[nVertices]);
		this->state = state;

		const std::vector<double>& openProbabilities = contextObj.getOperationalProbabilitiesD();
		
		//Get out which vertices are present, and the count of the total number of present vertices
		for(std::size_t i = 0; i < nVertices; i++)
		{
			boost::random::bernoulli_distribution<double> vertexDistribution(openProbabilities[i]);
			if(vertexDistribution(randomSource))
			{
				state[i].state = UNFIXED_ON;
			}
			else state[i].state = UNFIXED_OFF;
		}
	}
	observation::observation(context const& contextObj, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::basicConstructorType&)
		:contextObj(contextObj), state(state)
	{
	}
	observation::observation(const observation& other)
		: contextObj(other.contextObj), state(other.state)
	{}
	observation::observation(context const& contextObj, boost::shared_array<const vertexState> state)
		: contextObj(contextObj), state(state)
	{
	}
	observation::observation(observation&& other)
		:contextObj(other.contextObj)
	{
		state.swap(other.state);
	}
	context const& observation::getContext() const
	{
		return contextObj;
	}
	observation& observation::operator=(observation&& other)
	{
		if(&contextObj != &(other.contextObj)) throw std::runtime_error("Attempting to mix different contexts!");
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
	const context& observationWithContext::getContext() const
	{
		return *contextObj.get();
	}
}
