#ifndef ARGUMENTS_MPIR_HEADER_GUARD
#define ARGUMENTS_MPIR_HEADER_GUARD
#include <boost/program_options.hpp>
#include "includeNumerics.h"
#include "Context.h"
namespace residualConnectivity
{
	bool readProbabilityString(boost::program_options::variables_map& variableMap, mpfr_class& out, std::string& message);
	bool readContext(boost::program_options::variables_map& variableMap, Context& out, mpfr_class opProbability, std::string& message);
}
#endif
