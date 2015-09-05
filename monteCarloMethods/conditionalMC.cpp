#include "monteCarloMethods/conditionalMC.h"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/random/geometric_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include "observation.h"
#include "subObs/subObs.h"
#include "isSingleComponentWithRadius.h"
#include <boost/iterator/counting_iterator.hpp>
namespace discreteGermGrain
{	
	void conditionalMC(conditionalMCArgs& args)
	{
		std::size_t nVertices = boost::num_vertices(args.context.getGraph());
		boost::random::geometric_distribution<int, double> numberOffVertices(args.context.getOperationalProbability().convert_to<double>());

		mpfr_class total = 0;
		mpfr_class numeratorExpectedUpNumber = 0;
		mpfr_class q = 1 - args.context.getOperationalProbability();

		std::vector<int> connectedComponents(nVertices);
		std::vector<bool> isBoundary(nVertices);
		boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;
		
		std::vector<int> allVertices;
		allVertices.reserve(nVertices);

		//Cache of powers of Q
		std::vector<mpfr_class> powersOfQ;
		powersOfQ.reserve(nVertices + 1);
		powersOfQ.push_back(1);
		powersOfQ.push_back(q);
		for (int i = 2; i < (int)nVertices + 1; i++)
		{
			powersOfQ.push_back(mpfr_class(powersOfQ[i - 1] * q));
		}

		for(int i = 0; i < args.n; i++)
		{
			boost::shared_array<vertexState> state(new vertexState[nVertices]);
			std::fill(state.get(), state.get()+nVertices, vertexState::unfixed_off());
			//Pick random vertices and determine their state, until we get one that's fixed as on. 

			int nOff = numberOffVertices(args.randomSource);
			int componentSize = 0;
			int nBoundaryPoints = 0;
			if(nOff < (int)nVertices)
			{
				allVertices.clear();
				allVertices.insert(allVertices.begin(), boost::counting_iterator<int>(0), boost::counting_iterator<int>((int)nVertices));
				for(int i = 0; i < nOff; i++)
				{
					boost::random::uniform_int_distribution<> randomVertex(0, (int)allVertices.size()-1);
					int offVertexIndex = randomVertex(args.randomSource);
					state[allVertices[offVertexIndex]].state = FIXED_OFF;
					std::swap(*allVertices.rbegin(), allVertices[offVertexIndex]);
					allVertices.erase(allVertices.begin() + (allVertices.size()-1));
				}
				int onVertex;
				{
					boost::random::uniform_int_distribution<> randomVertex(0, (int)allVertices.size()-1);
					int onVertexIndex = randomVertex(args.randomSource);
					onVertex = allVertices[onVertexIndex];
					state[onVertex].state = FIXED_ON;
				}
				
				//represents partial knowledge
				::discreteGermGrain::subObs::subObs subObs(args.context, state);
				//generate complete knowledge
				::discreteGermGrain::observation obs = ::discreteGermGrain::subObs::getObservation<::discreteGermGrain::subObs::subObs>::get(subObs, args.randomSource);
				const vertexState* obsState = obs.getState();
				std::fill(connectedComponents.begin(), connectedComponents.end(), -1);
				
				std::vector<Context::inputGraph::vertex_descriptor> specified;
				specified.push_back(onVertex);
				
				isSingleComponentAllOn(args.context, obsState, connectedComponents, stack);
				for(std::size_t k = 0; k < nVertices; k++)
				{
					if(connectedComponents[k] == connectedComponents[onVertex]) componentSize++;
				}

				isBoundary.clear();
				isBoundary.resize(nVertices, false);

				Context::inputGraph::edge_iterator start, end;
				boost::tie(start, end) = boost::edges(args.context.getGraph());
				while(start != end)
				{
					if((obsState[start->m_source].state & ON_MASK) && (obsState[start->m_target].state & UNFIXED_OFF))
					{
						if(connectedComponents[start->m_source] == connectedComponents[onVertex])
						{
							isBoundary[start->m_target] = true;
						}
					}
					if((obsState[start->m_source].state & UNFIXED_OFF) && (obsState[start->m_target].state & ON_MASK))
					{
						if(connectedComponents[start->m_target] == connectedComponents[onVertex])
						{
							isBoundary[start->m_source] = true;
						}
					}
					start++;
				}
				for(std::size_t k = 0; k < nVertices; k++)
				{
					if(isBoundary[k]) nBoundaryPoints++;
				}

			}
			else
			{
				nOff = (int)nVertices;
				componentSize = 0;
				nBoundaryPoints = 0;
			}
			int power = ((int)nVertices - nOff) - componentSize - nBoundaryPoints;
			mpfr_class connectedProbability = powersOfQ[power];
			total += connectedProbability;
			numeratorExpectedUpNumber += componentSize * connectedProbability;
		}
		args.expectedUpNumber = numeratorExpectedUpNumber / total;
		args.estimate = total/args.n;
	}
}
