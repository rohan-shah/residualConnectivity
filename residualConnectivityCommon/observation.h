#ifndef RESIDUAL_CONNECTIVITY_OBS_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_OBS_HEADER_GUARD
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
#include "context.h"
#include "subObsTypes.h"
#include "subObs/getObservation.hpp"
namespace residualConnectivity
{
	class observation : public boost::noncopyable
	{
	public:
		friend class boost::serialization::access;
		template<class T> friend class ::residualConnectivity::subObs::getObservation;
		observation(context const& contextObj, boost::archive::binary_iarchive& archive);
		observation(context const& contextObj, boost::archive::text_iarchive& archive);
		observation(context const&, boost::mt19937& randomSource);
		observation(context const& contextObj, boost::shared_array<const vertexState> state);
		//A default constructor that fills all numeric members with -1. Somewhat dangerous to leave in here, but problems are due to the use of such invalid objects, it should be fairly obvious. 
		//observation(context const& contextObj);
		observation(observation&& other);
		observation& operator=(observation&& other);
		context const& getContext() const;
		const vertexState* getState() const;
	protected:
		observation(const observation& other);
		context const& contextObj;
		boost::shared_array<const vertexState> state;
	private:
		observation(context const& contextObj, boost::shared_array<const vertexState> state, ::residualConnectivity::obs::basicConstructorType&);
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		template<class Archive> void load(Archive& ar, const unsigned int version)
		{
			const context::inputGraph& graph = contextObj.getGraph();
			std::size_t nVertices = boost::num_vertices(graph);
			boost::shared_array<vertexState> state(new vertexState[nVertices]);
			ar & boost::serialization::make_array(state.get(), nVertices);
			this->state = state;
		};
		template<class Archive> void save(Archive& ar, const unsigned int version) const
		{
			const context::inputGraph& graph = contextObj.getGraph();
			std::size_t nVertices = boost::num_vertices(graph);
			ar & boost::serialization::make_array(state.get(), nVertices);
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
		const context& getContext() const;
	private:
		BOOST_SERIALIZATION_SPLIT_MEMBER()
		template<class Archive> void save(Archive& ar, const unsigned int version) const
		{
			std::string typeString = "residualConnectivityObsWithContext";
			ar << typeString;
			ar << obs->getContext();
			ar << *obs;
			typeString = "residualConnectivityObsWithContext_end";
			ar << typeString;
		}
		template<class Archive> void load(Archive& ar, const unsigned int version)
		{
			std::string typeString;
			ar >> typeString;
			if(typeString != "residualConnectivityObsWithContext")
			{
				throw std::runtime_error("Incorrect type specifier");
			}
			contextObj.reset(new context(ar));
			obs.reset(new observation(*contextObj.get(), ar));
			ar >> typeString;
			if(typeString != "residualConnectivityObsWithContext_end")
			{
				throw std::runtime_error("Incorrect type specifier");
			}
		}
		boost::shared_ptr<const context> contextObj;
		boost::shared_ptr<observation> obs;
	};
}
#endif
