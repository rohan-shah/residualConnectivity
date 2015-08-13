#ifndef DISCRETE_GERM_GRAIN_OBS_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_OBS_HEADER_GUARD
#include <boost/random/mersenne_twister.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <vector>
#include <fstream>
#include <boost/shared_ptr.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/access.hpp>
#include <boost/noncopyable.hpp>
#include "Context.h"
#include "subObsTypes.h"
#include "subObs/getObservation.hpp"
namespace discreteGermGrain
{
	class observation : public boost::noncopyable
	{
	public:
		friend class boost::serialization::access;
		template<class T> friend class ::discreteGermGrain::subObs::getObservation;
		observation(Context const& context, boost::archive::binary_iarchive& archive);
		observation(Context const& context, boost::archive::text_iarchive& archive);
		observation(Context const&, boost::mt19937& randomSource);
		observation(Context const& context, boost::shared_array<const vertexState> state);
		//A default constructor that fills all numeric members with -1. Somewhat dangerous to leave in here, but problems are due to the use of such invalid objects, it should be fairly obvious. 
		//observation(Context const& context);
		observation(observation&& other) noexcept;
		observation& operator=(observation&& other);
		Context const& getContext() const;
		const vertexState* getState() const;
	protected:
		observation(const observation& other);
		Context const& context;
		boost::shared_array<const vertexState> state;
	private:
		observation(Context const& context, boost::shared_array<const vertexState> state, ::discreteGermGrain::obs::basicConstructorType&);
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		template<class Archive> void load(Archive& ar, const unsigned int version)
		{
			boost::shared_array<vertexState> state(new vertexState[context.nVertices()]);
			ar & boost::serialization::make_array(state.get(), context.nVertices());
			this->state = state;
		};
		template<class Archive> void save(Archive& ar, const unsigned int version) const
		{
			ar & boost::serialization::make_array(state.get(), context.nVertices());
		}
	};
	class observationWithContext
	{
	public:
		observationWithContext(boost::archive::text_iarchive& ar)
		{
			ar >> *this;
		}
		observationWithContext(boost::archive::binary_iarchive& ar)
		{
			ar >> *this;
		}
		observationWithContext(const observationWithContext& other);
		friend class boost::serialization::access;
		const observation& getObs() const;
		const Context& getContext() const;
	private:
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		template<class Archive> void save(Archive& ar, const unsigned int version) const
		{
			std::string typeString = "discreteGermGrainObsWithContext";
			ar << typeString;
			ar << obs->getContext();
			ar << *obs;
			typeString = "discreteGermGrainObsWithContext_end";
			ar << typeString;
		}
		template<class Archive> void load(Archive& ar, const unsigned int version)
		{
			std::string typeString;
			ar >> typeString;
			if(typeString != "discreteGermGrainObsWithContext")
			{
				throw std::runtime_error("Incorrect type specifier");
			}
			context.reset(new Context(ar));
			obs.reset(new observation(*context.get(), ar));
			ar >> typeString;
			if(typeString != "discreteGermGrainObsWithContext_end")
			{
				throw std::runtime_error("Incorrect type specifier");
			}
		}
		boost::shared_ptr<const Context> context;
		boost::shared_ptr<observation> obs;
	};
}
#endif
