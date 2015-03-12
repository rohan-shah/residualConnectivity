#include "customizedEigen.h"
namespace Eigen
{
	::mpz_class operator*(const ::discreteGermGrain::binaryValue& lhs, const ::mpz_class& rhs)
	{
		if(lhs.value) return rhs;
		return 0;
	}
	::mpz_class operator*(const ::mpz_class& lhs, const ::discreteGermGrain::binaryValue& rhs)
	{
		if(rhs.value) return lhs;
		return 0;
	}
}
