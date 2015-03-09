#ifndef DISCRETE_GERM_GRAIN_OBS_HEADER_GUARD
#define DISCRETE_GERM_GRAIN_OBS_HEADER_GUARD
#include <boost/random/mersenne_twister.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <vector>
#include <fstream>
#include <boost/shared_ptr.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/access.hpp>
#include "Context.h"
namespace discreteGermGrain
{
	class DiscreteGermGrainSubObs;
	class DiscreteGermGrainObs
	{
	public:
		friend class boost::serialization::access;
		DiscreteGermGrainObs(Context const&, boost::mt19937& randomSource);
		DiscreteGermGrainObs(Context const& context, boost::shared_array<const vertexState> partialKnowledge, boost::mt19937& randomSource);
		//A default constructor that fills all numeric members with -1. Somewhat dangerous to leave in here, but problems are due to the use of such invalid objects, it should be fairly obvious. 
		//DiscreteGermGrainObs(Context const& context);
		DiscreteGermGrainObs(DiscreteGermGrainObs&& other);
		DiscreteGermGrainObs(const DiscreteGermGrainObs& other);
		DiscreteGermGrainObs(Context const& context, boost::shared_array<const vertexState> state);
		DiscreteGermGrainObs& operator=(DiscreteGermGrainObs&& other);
		DiscreteGermGrainObs& operator=(const DiscreteGermGrainObs& other);
		Context const& getContext() const;
		const vertexState* getState() const;
		DiscreteGermGrainSubObs getSubObservation(int radius) const;
	protected:
		Context const& context;
		boost::shared_array<const vertexState> state;
		typedef std::map<int, boost::shared_array<const vertexState> > cacheVectorType;
		mutable cacheVectorType subPointsCache;
	private:
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		template<class Archive> void load(Archive& ar, const unsigned int version)
		{
			state.reset(new vertexState[context.nVertices()]);
			ar & boost::serialization::make_array(state.get(), context.nVertices());
		};
		template<class Archive> void save(Archive& ar, const unsigned int version) const
		{
			ar & boost::serialization::make_array(state.get(), context.nVertices());
		}
	};
}
#endif
