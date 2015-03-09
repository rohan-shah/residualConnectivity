#include <boost/program_options.hpp>
#include <boost/random/mersenne_twister.hpp>
#include "DiscreteGermGrainObs.h"
#include <boost/algorithm/string.hpp>
#include "splittingViewer.h"
#include "DiscreteGermGrainSubObs.h"
#include "isSingleComponentWithRadius.h"
#include <boost/graph/graphml.hpp>
#include "arguments.h"
#include <QApplication>
#if defined(_WIN32)
	#include <Windows.h>
	#if defined(_MSC_VER)
		#include "windowsConsoleOutput.h"
	#endif
#endif
namespace discreteGermGrain
{
#if defined(_WIN32)
	void registerQTPluginDir()
	{
		static bool pluginDir = false;
		if(!pluginDir)
		{
			WCHAR pathArray[500];
			GetModuleFileNameW(NULL, pathArray, 500);
			int error = GetLastError();
			if(error != ERROR_SUCCESS)
			{
						exit(-1);
			}
			std::wstring path(&(pathArray[0]));

			path.erase(std::find(path.rbegin(), path.rend(), '\\').base(), path.end());
			QApplication::addLibraryPath(QString::fromStdWString(path));
			pluginDir = true;
		}
	}
#endif
	int main(int argc, char** argv)
	{
		boost::program_options::options_description options("Usage");
		options.add_options()
			("gridGraph", boost::program_options::value<int>(), "(int) The dimension of the square grid graph to use. Incompatible with graphFile and torusGraph. ")
			("graphFile", boost::program_options::value<std::string>(), "(string) The path to a graphml file. Incompatible with gridGraph")
			("probability", boost::program_options::value<float>(), "(float) The probability that a vertex is open")
			("seed", boost::program_options::value<int>(), "(int) The random seed used to generate the random graphs")
			("initialRadius", boost::program_options::value<int>(), "(int) The initial radius to use for the splitting algorithm")
			("pointSize", boost::program_options::value<float>(), "(float) The size of graph vertices. Defaults to 0.1")
			("help", "Display this message");
#if defined(_WIN32) && defined(_MSC_VER)
		redirectConsoleOutput();
#endif
		boost::program_options::variables_map variableMap;
		try
		{
			boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options), variableMap);
		}
		catch(boost::program_options::error& ee)
		{
			std::cerr << "Error parsing command line arguments: " << ee.what() << std::endl << std::endl;
			std::cerr << options << std::endl;
			return -1;
		}
		if(variableMap.count("help") > 0)
		{
			std::cout << options << std::endl;
			return 0;
		}
#if defined(_WIN32)
		registerQTPluginDir();
#endif
		QApplication app(argc, argv);
		boost::mt19937 randomSource;
		readSeed(variableMap, randomSource);

		std::string message;
		double probability;
		if(!readProbability(variableMap, probability, message))
		{
			std::cout << message << std::endl;
			return 0;
		}
		Context context = Context::gridContext(1, probability);
		if(!readContext(variableMap, context, probability, message))
		{
			std::cout << message << std::endl;
			return 0;
		}

		int initialRadius;
		if(!readInitialRadius(variableMap, initialRadius))
		{
			return 0;
		}

		float pointSize = 0.1f;
		if(variableMap.count("pointSize") > 1)
		{
			pointSize = variableMap["pointSize"].as<float>();
		}

		std::vector<DiscreteGermGrainObs> observations;
		std::vector<int> scratchMemory;
		boost::detail::depth_first_visit_restricted_impl_helper<Context::inputGraph>::stackType stack;
		while(true)
		{
			DiscreteGermGrainObs obs(context, randomSource);
			DiscreteGermGrainSubObs subObs(obs.getSubObservation(initialRadius));
			if(isSingleComponentPossible(context, subObs.getState(), scratchMemory, stack))
			{
				splittingViewer viewer(obs, initialRadius, randomSource, pointSize);
				viewer.show();
				app.exec();
				return 0;
			}
		}
		return 0;
	}
}
int main(int argc, char** argv)
{
	return discreteGermGrain::main(argc, argv);
}
