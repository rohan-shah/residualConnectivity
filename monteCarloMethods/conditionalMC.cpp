#include "monteCarloMethods/conditionalMC.h"
#include <boost/random/bernoulli_distribution.hpp>
#include <boost/random/geometric_distribution.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include "observation.h"
#include "subObs/subObs.h"
#include "isSingleComponentWithRadius.h"
#include <boost/iterator/counting_iterator.hpp>
#include <boost/range/algorithm/random_shuffle.hpp>
#include <boost/random/random_number_generator.hpp>
namespace residualConnectivity
{	
	void conditionalMC(conditionalMCArgs& args)
	{
		std::size_t nVertices = boost::num_vertices(args.contextObj.getGraph());
		const std::vector<double>& operationalProbabilitiesD = args.contextObj.getOperationalProbabilitiesD();
		const std::vector<mpfr_class>& operationalProbabilities = args.contextObj.getOperationalProbabilities();
		boost::random_number_generator<boost::mt19937> generator(args.randomSource);

		mpfr_class total = 0;
		mpfr_class numeratorExpectedUpNumber = 0;

		std::vector<int> connectedComponents(nVertices);
		std::vector<bool> isBoundary(nVertices);
		boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType stack;
		
		std::vector<int> allVertices;
		allVertices.reserve(nVertices);

		for(int i = 0; i < args.n; i++)
		{
			boost::shared_array<vertexState> state(new vertexState[nVertices]);
			std::fill(state.get(), state.get()+nVertices, vertexState::unfixed_off());
			//Pick random vertices and determine their state, until we get one that's fixed as on. 
			allVertices.clear();
			allVertices.insert(allVertices.begin(), boost::counting_iterator<int>(0), boost::counting_iterator<int>((int)nVertices));
			boost::random_shuffle(allVertices, generator);
			int firstOn = 0;
			for (; firstOn < (int)nVertices; firstOn++)
			{
				boost::random::bernoulli_distribution<double> vertexDistribution(operationalProbabilitiesD[allVertices[firstOn]]);
				if (vertexDistribution(args.randomSource))
				{
					state[allVertices[firstOn]].state = FIXED_ON;
					break;
				}
				else
				{
					state[allVertices[firstOn]].state = FIXED_OFF;
				}
			}

			if (firstOn == (int)nVertices - 1)
			{
				total += 1;
				numeratorExpectedUpNumber += 1;
			}
			else if(firstOn < (int)nVertices)
			{
				int onVertex = allVertices[firstOn];
				
				//represents partial knowledge
				::residualConnectivity::subObs::subObs subObs(args.contextObj, state);
				//generate complete knowledge
				::residualConnectivity::observation obs = ::residualConnectivity::subObs::getObservation<::residualConnectivity::subObs::subObs>::get(subObs, args.randomSource);
				const vertexState* obsState = obs.getState();
				std::fill(connectedComponents.begin(), connectedComponents.end(), -1);
				
				isSingleComponentAllOn(args.contextObj, obsState, connectedComponents, stack);
				int componentSize = 0;
				for(std::size_t k = 0; k < nVertices; k++)
				{
					if(connectedComponents[k] == connectedComponents[onVertex]) componentSize++;
				}

				isBoundary.clear();
				isBoundary.resize(nVertices, false);

				context::inputGraph::edge_iterator start, end;
				boost::tie(start, end) = boost::edges(args.contextObj.getGraph());
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
				mpfr_class connectedProbability = 1;
				for(std::size_t k = 0; k < nVertices; k++)
				{
					if (!isBoundary[k] && (obsState[k].state != UNFIXED_ON || connectedComponents[k] != connectedComponents[onVertex]) && obsState[k].state != FIXED_OFF && obsState[k].state != FIXED_ON)
					{
						connectedProbability *= (1 - operationalProbabilities[k]);
					}
				}
				total += connectedProbability;
				numeratorExpectedUpNumber += connectedProbability*componentSize;
			}
			else
			{
				total += 1;
				numeratorExpectedUpNumber += 0;
			}
		}
		args.expectedUpNumber = numeratorExpectedUpNumber / total;
		args.estimate = total/args.n;
	}
}
