#ifndef INCLUDE_MPFR_HEADER_GUARD
#define INCLUDE_MPFR_HEADER_GUARD
//#include "mpreal.h"
//typedef mpfr::mpreal mpfr_class;
#include <boost/multiprecision/mpfr.hpp>
#include <boost/multiprecision/gmp.hpp>
typedef boost::multiprecision::static_mpfr_float_50 mpfr_class;
typedef boost::multiprecision::mpz_int mpz_class;
#endif
