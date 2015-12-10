#include "empiricalDistribution.h"
#include <stdexcept>
#include <fstream>
namespace residualConnectivity
{
	empiricalDistribution::empiricalDistribution(bool _isWeighted, std::size_t nVertices, const Context& context)
	:nVertices(nVertices), sampleSize(0), _isWeighted(_isWeighted), externalContext(&context)
	{}
	void empiricalDistribution::hintDataCount(std::size_t size)
	{
		if(_isWeighted) weights.reserve(size);
		std::size_t nBitsRequired = size*nVertices;
		std::size_t nWholeIntsRequired = nBitsRequired / (8*sizeof(int));
		if(nWholeIntsRequired * sizeof(int)*8 == nBitsRequired)
		{
			data.reserve(nWholeIntsRequired);
		}
		else data.reserve(nWholeIntsRequired+1);
	}
	bool empiricalDistribution::isWeighted() const
	{
		return _isWeighted;
	}
	void empiricalDistribution::expand(std::size_t count, std::vector<int>& output) const
	{
		if(output.size() != nVertices) throw std::runtime_error("Wrong number of elements in vector passed to empiricalDistribution::expand");
		binaryDataSet1::expand(count, output);
	}
	double empiricalDistribution::getWeight(std::size_t index) const
	{
		if(!_isWeighted) throw std::runtime_error("Trying to get weight of unweighted empirical distribution");
		return weights[index];
	}
	void empiricalDistribution::add(const vertexState* state)
	{
		if(_isWeighted)
		{
			throw std::runtime_error("Attempting to add unweighted point to weighted empirical distribution");
		}
		binaryDataSet1::add(state, nVertices);
		sampleSize++;
	}
	void empiricalDistribution::add(const vertexState* state, double weight)
	{
		if(!_isWeighted)
		{
			throw std::runtime_error("Attempting to add weighted point to unweighted empirical distribution");
		}
		binaryDataSet1::add(state, nVertices);
		weights.push_back(weight);
		sampleSize++;
	}
	empiricalDistribution& empiricalDistribution::operator=(empiricalDistribution&& other)
	{
		*static_cast<binaryDataSet1*>(this) = static_cast<binaryDataSet1&&>(other);
		_isWeighted = other._isWeighted;
		weights.swap(other.weights);
		sampleSize = other.sampleSize;
		externalContext = other.externalContext;
		containedContext = other.containedContext;
		return *this;
	}
	empiricalDistribution::empiricalDistribution(empiricalDistribution&& other)
		:binaryDataSet1(std::move(other))
	{
		_isWeighted = other._isWeighted;
		weights.swap(other.weights);
		sampleSize = other.sampleSize;
		externalContext = other.externalContext;
		containedContext = other.containedContext;
	}
	empiricalDistribution::empiricalDistribution()
	{}
	std::size_t empiricalDistribution::getNSamples() const
	{
		return sampleSize;
	}
	std::size_t empiricalDistribution::getNEdges() const
	{
		return nVertices;
	}
	const Context& empiricalDistribution::getContext() const
	{
		if(externalContext) return *externalContext;
		return *containedContext.get();
	}
}
