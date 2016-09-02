#include "context.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/graph/connected_components.hpp>
#include "connected_components_restricted.hpp"
#include "monteCarloMethods/recursiveVarianceReduction.h"
namespace residualConnectivity
{
	void recursiveVarianceReduction(recursiveVarianceReductionArgs& args)
	{
		std::size_t nVertices = args.contextObj.nVertices();
		const std::vector<mpfr_class>& opProbabilities = args.contextObj.getOperationalProbabilities();
		//Cache powers of opProbability and inopProbability
		std::vector<mpfr_class> opProbabilityPowers(nVertices+1, 1);
		for(std::size_t i = 0; i < nVertices+1; i++)
		{
			for (std::size_t j = i; j < nVertices; j++)
			{
				opProbabilityPowers[i] *= opProbabilities[j];
			}
		}
		//unifrom distribution
		boost::random::uniform_01<double> uniformDistribution;
		const context::inputGraph& graph = args.contextObj.getGraph();

		//scratch memory, to determine whether the base graph is connected
		std::vector<int> components(nVertices);
		//more scratch memory
		std::vector<typename boost::graph_traits<context::inputGraph>::vertex_descriptor> allVertices;
		for(std::size_t i = 0; i < nVertices; i++)allVertices.push_back(i);
		bool totalGraphConnected = boost::connected_components(graph, &(components[0])) == 1;

		std::vector<boost::default_color_type> colorVector(nVertices);
		typedef boost::color_traits<boost::default_color_type> Color;

		boost::detail::depth_first_visit_restricted_impl_helper<context::inputGraph>::stackType stack;

		std::vector<bool> iterativeVertexStates(nVertices);
		args.estimate = 0;
		for(int i = 0; i < args.n; i++)
		{
			iterativeVertexStates.clear();
			iterativeVertexStates.resize(nVertices, true);

			mpfr_class currentPart = 0;
			mpfr_class currentMultiple = 1;
			std::size_t currentPosition = 0;
			bool currentConnected = totalGraphConnected;
			while(true)
			{
				//Terminating conditions. First, if we have currentPart == nVertices - 1 then we're done. 
				if(currentPosition == nVertices)
				{
					if(currentConnected) currentPart += currentMultiple;
					break;
				}
				//recursive part
				if(currentConnected)
				{
					currentPart += currentMultiple * opProbabilityPowers[currentPosition];
				}
				mpfr_class extraPart = (1 - opProbabilityPowers[currentPosition]);
				currentMultiple *= extraPart;

				std::size_t increment = 0;
				mpfr_class randomValue = extraPart * uniformDistribution(args.randomSource);
				mpfr_class prod = 1;
				for(; increment < nVertices - currentPosition; increment++)
				{
					mpfr_class tmp = prod*(1 - opProbabilities[currentPosition + increment]);
					if (randomValue < tmp) break;
					randomValue -= tmp;
					prod *= opProbabilities[currentPosition + increment];
				}
				//mark currentPosition + increment as off. 
				iterativeVertexStates[currentPosition + increment] = false;
				currentPosition += increment;
				currentPosition++;
				//Set the colour vector appropriately
				for(std::size_t j = 0; j < nVertices; j++)
				{
					if(iterativeVertexStates[j]) colorVector[j] = Color::white();
					else colorVector[j] = Color::black();
				}
				//update the currentConnected variable
				currentConnected = boost::connected_components_restricted(graph, &(components[0]), &(colorVector[0]), stack, allVertices) <= 1;
			}
			args.estimate += currentPart;
		}
		args.estimate /= args.n;
	}
}
