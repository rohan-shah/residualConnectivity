#include "transferMatrixCommon/customizedEigen.h"
namespace Eigen
{
	::residualConnectivity::mpz_class operator*(const ::residualConnectivity::binaryValue& lhs, const ::residualConnectivity::mpz_class& rhs)
	{
		if(lhs.value) return rhs;
		return 0;
	}
	::residualConnectivity::mpz_class operator*(const ::residualConnectivity::mpz_class& lhs, const ::residualConnectivity::binaryValue& rhs)
	{
		if(rhs.value) return lhs;
		return 0;
	}
}
