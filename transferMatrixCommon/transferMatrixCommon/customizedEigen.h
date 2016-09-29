#ifndef CUSTOMIZED_EIGEN_HEADER_GUARD
#define CUSTOMIZED_EIGEN_HEADER_GUARD
#include "countSubgraphsCommon.h"
namespace residualConnectivity
{
	struct binaryValue
	{
		binaryValue()
			:value(0)
		{}
		binaryValue(int value)
			:value(value)
		{}
		binaryValue(unsigned long value)
			:value((int)value)
		{}
		binaryValue(long value)
			:value((int)value)
		{}
		int value;
	};
	typedef ::Eigen::Matrix<binaryValue, -1, -1, ::Eigen::RowMajor, -1, -1> TransitionMatrix;
}
namespace Eigen
{
	namespace internal
	{
		template<> struct scalar_product_traits<::residualConnectivity::mpz_class, ::residualConnectivity::binaryValue>
		{
			enum
			{
				Defined = 1
			};
			typedef ::residualConnectivity::mpz_class ReturnType;
		};
		template<> struct scalar_product_traits<::residualConnectivity::binaryValue, ::residualConnectivity::mpz_class>
		{
			enum
			{
				Defined = 1
			};
			typedef ::residualConnectivity::mpz_class ReturnType;
		};
	}
	::residualConnectivity::mpz_class operator*(const ::residualConnectivity::binaryValue& lhs, const ::residualConnectivity::mpz_class& rhs);
/*	{
		if(lhs.value) return rhs;
		return 0;
	}*/
	::residualConnectivity::mpz_class operator*(const ::residualConnectivity::mpz_class& lhs, const ::residualConnectivity::binaryValue& rhs);
/*	{
		if(rhs.value) return lhs;
		return 0;
	}*/
	namespace internal
	{
		template<> struct conj_helper<::residualConnectivity::binaryValue, ::residualConnectivity::mpz_class, false, false>
		{
			EIGEN_STRONG_INLINE ::residualConnectivity::mpz_class pmadd(const ::residualConnectivity::binaryValue& a, const ::residualConnectivity::mpz_class& b, const ::residualConnectivity::mpz_class& c)
			{
				if(a.value) return b + c;
				return c;
			}
			EIGEN_STRONG_INLINE ::residualConnectivity::mpz_class padd(const ::residualConnectivity::binaryValue& a, const ::residualConnectivity::mpz_class& b)
			{
				if(a.value) return b+1;
				return b;
			}
			EIGEN_STRONG_INLINE ::residualConnectivity::mpz_class pmul(const ::residualConnectivity::binaryValue& a, const ::residualConnectivity::mpz_class& b)
			{
				if(a.value) return b;
				return 0;
			}
		};
		template<> struct conj_helper<::residualConnectivity::mpz_class, ::residualConnectivity::binaryValue, false, false>
		{
			EIGEN_STRONG_INLINE ::residualConnectivity::mpz_class pmadd(const ::residualConnectivity::mpz_class& a, const ::residualConnectivity::binaryValue& b, const ::residualConnectivity::mpz_class& c)
			{
				if(b.value) return a + c;
				return c;
			}
			EIGEN_STRONG_INLINE ::residualConnectivity::mpz_class padd(const ::residualConnectivity::mpz_class& a, const ::residualConnectivity::binaryValue& b)
			{
				if(b.value) return a+1;
				return a;
			}
			EIGEN_STRONG_INLINE ::residualConnectivity::mpz_class pmul(const ::residualConnectivity::mpz_class& a, const ::residualConnectivity::binaryValue& b)
			{
				if(b.value) return a;
				return 0;
			}
		};
	}
//	template<> GeneralProduct<::residualConnectivity::LargeDenseIntMatrix, ::residualConnectivity::TransitionMatrix, 5>::GeneralProduct(const ::residualConnectivity::LargeDenseIntMatrix& lhs, const ::residualConnectivity::TransitionMatrix& rhs);
//                :GeneralProduct<::residualConnectivity::LargeDenseIntMatrix, ::residualConnectivity::TransitionMatrix, 5>::Base(lhs, rhs)
//         {}
}
#endif
