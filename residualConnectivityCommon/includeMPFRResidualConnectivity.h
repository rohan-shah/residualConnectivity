#ifndef INCLUDE_MPFR_RESIDUAL_CONNECTEDNESS_HEADER_GUARD
#define INCLUDE_MPFR_RESIDUAL_CONNECTEDNESS_HEADER_GUARD
//#include "mpreal.h"
//typedef mpfr::mpreal mpfr_class;
#include <boost/multiprecision/mpfr.hpp>
#include <boost/multiprecision/gmp.hpp>
namespace residualConnectivity
{
	typedef boost::multiprecision::mpfr_float mpfr_class;
	typedef boost::multiprecision::mpz_int mpz_class;
}
#endif
