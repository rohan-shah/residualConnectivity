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
	class DiscreteGermGrainSubObs;
	class DiscreteGermGrainObs : public boost::noncopyable
	{
	public:
		friend class boost::serialization::access;
		template<class T> friend class ::discreteGermGrain::subObs::getObservation;
		DiscreteGermGrainObs(Context const& context, boost::archive::binary_iarchive& archive);
		DiscreteGermGrainObs(Context const& context, boost::archive::text_iarchive& archive);
		DiscreteGermGrainObs(Context const&, boost::mt19937& randomSource);
		DiscreteGermGrainObs(Context const& context, boost::shared_array<const vertexState> state);
		//A default constructor that fills all numeric members with -1. Somewhat dangerous to leave in here, but problems are due to the use of such invalid objects, it should be fairly obvious. 
		//DiscreteGermGrainObs(Context const& context);
		DiscreteGermGrainObs(DiscreteGermGrainObs&& other);
		DiscreteGermGrainObs& operator=(DiscreteGermGrainObs&& other);
		Context const& getContext() const;
		const vertexState* getState() const;
	protected:
		Context const& context;
		boost::shared_array<const vertexState> state;
	private:
		DiscreteGermGrainObs(Context const& context, boost::shared_array<const vertexState> state, ::discreteGermGrain::obs::basicConstructorType&);
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
}
#endif
