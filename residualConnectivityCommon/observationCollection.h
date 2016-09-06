#ifndef RESIDUAL_CONNECTIVITY_SUBOBS_COLLECTION_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_SUBOBS_COLLECTION_HEADER_GUARD
#include "vertexState.h"
#include "empiricalDistribution.h"
#include "observation.h"
#include "binaryDataSet.h"
#include <boost/serialization/access.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
namespace residualConnectivity
{
	class observationCollection : protected binaryDataSet2, public boost::noncopyable
	{
	public:
		friend class boost::serialization::access;
		observationCollection(context const* externalContext, double radius);
		observationCollection(boost::archive::binary_iarchive& ar);
		observationCollection(boost::archive::text_iarchive& ar);
		observationCollection(observationCollection&& other);
		observationCollection& operator=(observationCollection&& other);
		observationCollection(const empiricalDistribution& other);
		void add(const observation& subObs);
		const context& getContext() const;
		void expand(int count, boost::shared_array<vertexState> state) const;
		double getRadius() const;
		std::size_t getSampleSize() const;
		void reserve(std::size_t count);
	private:
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		template<class Archive> void save(Archive& ar, const unsigned int version) const
		{
			std::string typeString = "residualConnectivitySubObsCollection";
			ar << typeString;
			ar << sampleSize;
			ar << radius;
			if(containedContext)
			{
				ar << *containedContext.get();
			}
			else ar << *externalContext;
			ar << *static_cast<const binaryDataSet2*>(this);
			typeString = "residualConnectivitySubObsCollection_end";
			ar << typeString;
		}
		template<class Archive> void load(Archive& ar, const unsigned int version)
		{
			std::string typeString;
			ar >> typeString;
			if(typeString != "residualConnectivitySubObsCollection")
			{
				throw std::runtime_error("Incorrect type specifier");
			}
			ar >> sampleSize;
			ar >> radius;
			containedContext.reset(new context(ar));
			ar >> *static_cast<binaryDataSet2*>(this);
			ar >> typeString;
			if(typeString != "residualConnectivitySubObsCollection_end")
			{
				throw std::runtime_error("Incorrect type specifier");
			}
		}
		std::size_t sampleSize;
		std::shared_ptr<context> containedContext;
		context const* externalContext;
		double radius;
	};
}
#endif
