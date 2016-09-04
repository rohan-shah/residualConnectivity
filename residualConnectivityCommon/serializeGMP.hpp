#ifndef RESIDUAL_CONNECTIVITY_SERIALIZE_GMP_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_SERIALIZE_GMP_HEADER_GUARD
#include "includeMPFRResidualConnectivity.h"
#include <boost/serialization/split_free.hpp>
#include <iomanip>
namespace boost
{
	namespace serialization
	{
		template <class Archive> inline void serialize(Archive& ar, ::residualConnectivity::mpfr_class& number, const unsigned int version)
		{
			split_free(ar, number, version);
		}
		template <class Archive> void save(Archive & ar, const ::residualConnectivity::mpfr_class & number, const unsigned int version)
		{
			mp_exp_t exponent;
			char* resultCStr = mpfr_get_str(NULL, &exponent, 10, 0, number.backend().data(), MPFR_RNDN);
			std::string resultStr = resultCStr;
			std::string outputString = (resultStr + "@" + boost::lexical_cast<std::string>(exponent - (mp_exp_t)resultStr.size()));
			ar << outputString;
			free(resultCStr);
		}
		template <class Archive> void load(Archive & ar, ::residualConnectivity::mpfr_class & number, const unsigned int version)
		{
			std::string resultStr;
			ar >> resultStr;
			number = ::residualConnectivity::mpfr_class(resultStr);
		}
	}
}
#endif
