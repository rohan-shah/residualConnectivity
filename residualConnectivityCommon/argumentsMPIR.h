#ifndef ARGUMENTS_MPIR_HEADER_GUARD
#define ARGUMENTS_MPIR_HEADER_GUARD
#include <boost/program_options.hpp>
#include "includeNumerics.h"
namespace discreteGermGrain
{
	bool readProbabilityString(boost::program_options::variables_map& variableMap, mpfr_class& out, std::string& message);
}
#endif
