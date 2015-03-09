#ifndef ARGUMENTS_MPIR_HEADER_GUARD
#define ARGUMENTS_MPIR_HEADER_GUARD
#include <boost/program_options.hpp>
#include <mpir.h>
#include <mpirxx.h>
namespace discreteGermGrain
{
	bool readProbabilityString(boost::program_options::variables_map& variableMap, mpf_class& out);
}
#endif