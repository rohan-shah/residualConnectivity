#include "observationCollection.h"
#include <boost/scoped_array.hpp>
namespace discreteGermGrain
{
	observationCollection::observationCollection(Context const* externalContext, double radius)
		:sampleSize(0), externalContext(externalContext), radius(radius)
	{}
	observationCollection::observationCollection(boost::archive::binary_iarchive& ar)
		:sampleSize(0), externalContext(NULL)
	{
		ar >> *this;	
	}
	observationCollection::observationCollection(boost::archive::text_iarchive& ar)
		:sampleSize(0), externalContext(NULL)
	{
		ar >> *this;
	}
	observationCollection::observationCollection(const empiricalDistribution& other)
		:sampleSize(other.getNSamples()), radius(0)
	{
		externalContext = &other.getContext();
		std::size_t nVertices = externalContext->nVertices();
		std::vector<int> vectorStates(nVertices);
		boost::scoped_array<vertexState> state(new vertexState[nVertices]);
		for(std::size_t i = 0; i < sampleSize; i++)
		{
			other.expand(i, vectorStates);
			for(std::size_t j = 0; j < nVertices; j++)
			{
				if(vectorStates[j]) state[j].state = FIXED_ON;
				else state[j].state = FIXED_OFF;
			}
			static_cast<binaryDataSet2*>(this)->add(state.get(), nVertices);
		}
	}
	void observationCollection::reserve(std::size_t count)
	{
		const Context* currentContext;
		if(externalContext) currentContext = externalContext;
		else currentContext = &*containedContext;
		binaryDataSet2::reserve(count * currentContext->nVertices());
	}
	void observationCollection::expand(int count, boost::shared_array<vertexState> state) const
	{
		const Context* currentContext;
		if(externalContext) currentContext = externalContext;
		else currentContext = &*containedContext;

		binaryDataSet2::expand(count, state.get(), currentContext->nVertices());
	}
	void observationCollection::add(const observation& obs)
	{
		//Check that certain key variables are the same - Checking by pointer should be sufficient.
		Context const& obsContext = obs.getContext();
		const Context* currentContext;
		if(externalContext) currentContext = externalContext;
		else currentContext = &*containedContext;

		if(*obsContext.getShortestDistances() != *currentContext->getShortestDistances() || &(obsContext.getOperationalProbability()) != &(currentContext->getOperationalProbability()) || obsContext.nVertices() != currentContext->nVertices())
		{
			throw std::runtime_error("observation object added to observationCollection had wrong Context object");
		}
		static_cast<binaryDataSet2*>(this)->add(obs.getState(), obs.getContext().nVertices());
		sampleSize++;
	}
	double observationCollection::getRadius() const
	{
		return radius;
	}
	const Context& observationCollection::getContext() const
	{
		if(containedContext) return *containedContext.get();
		if(externalContext) return *externalContext;
		throw std::runtime_error("Invalid state for observationCollection");
	}
	observationCollection::observationCollection(observationCollection&& other)
		:binaryDataSet2(std::move(other)), sampleSize(other.sampleSize), containedContext(other.containedContext), externalContext(other.externalContext), radius(other.radius)
	{}
	observationCollection& observationCollection::operator=(observationCollection&& other)
	{
		sampleSize = other.sampleSize;
		containedContext = other.containedContext;
		externalContext = other.externalContext;
		radius = other.radius;
		*static_cast<binaryDataSet2*>(this) = std::move(static_cast<binaryDataSet2&&>(other));
		return *this;
	}
	std::size_t observationCollection::getSampleSize() const
	{
		return sampleSize;
	}
}
