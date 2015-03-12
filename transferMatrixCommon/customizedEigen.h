#ifndef CUSTOMIZED_EIGEN_HEADER_GUARD
#define CUSTOMIZED_EIGEN_HEADER_GUARD
#include "countSubgraphsCommon.h"
namespace discreteGermGrain
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
		template<> struct scalar_product_traits<::mpz_class, ::discreteGermGrain::binaryValue>
		{
			enum
			{
				Defined = 1
			};
			typedef ::mpz_class ReturnType;
		};
		template<> struct scalar_product_traits<::discreteGermGrain::binaryValue, ::mpz_class>
		{
			enum
			{
				Defined = 1
			};
			typedef ::mpz_class ReturnType;
		};
	}
	::mpz_class operator*(const ::discreteGermGrain::binaryValue& lhs, const ::mpz_class& rhs);
/*	{
		if(lhs.value) return rhs;
		return 0;
	}*/
	::mpz_class operator*(const ::mpz_class& lhs, const ::discreteGermGrain::binaryValue& rhs);
/*	{
		if(rhs.value) return lhs;
		return 0;
	}*/
	namespace internal
	{
		template<> struct conj_helper<::discreteGermGrain::binaryValue, ::mpz_class, false, false>
		{
			EIGEN_STRONG_INLINE ::mpz_class pmadd(const ::discreteGermGrain::binaryValue& a, const ::mpz_class& b, const ::mpz_class& c)
			{
				if(a.value) return b + c;
				return c;
			}
			EIGEN_STRONG_INLINE ::mpz_class padd(const ::discreteGermGrain::binaryValue& a, const ::mpz_class& b)
			{
				if(a.value) return b+1;
				return b;
			}
			EIGEN_STRONG_INLINE ::mpz_class pmul(const ::discreteGermGrain::binaryValue& a, const ::mpz_class& b)
			{
				if(a.value) return b;
				return 0;
			}
		};
		template<> struct conj_helper<::mpz_class, ::discreteGermGrain::binaryValue, false, false>
		{
			EIGEN_STRONG_INLINE ::mpz_class pmadd(const ::mpz_class& a, const ::discreteGermGrain::binaryValue& b, const ::mpz_class& c)
			{
				if(b.value) return a + c;
				return c;
			}
			EIGEN_STRONG_INLINE ::mpz_class padd(const ::mpz_class& a, const ::discreteGermGrain::binaryValue& b)
			{
				if(b.value) return a+1;
				return a;
			}
			EIGEN_STRONG_INLINE ::mpz_class pmul(const ::mpz_class& a, const ::discreteGermGrain::binaryValue& b)
			{
				if(b.value) return a;
				return 0;
			}
		};
	}
	template<> GeneralProduct<::discreteGermGrain::LargeDenseIntMatrix, ::discreteGermGrain::TransitionMatrix, 5>::GeneralProduct(const ::discreteGermGrain::LargeDenseIntMatrix& lhs, const ::discreteGermGrain::TransitionMatrix& rhs);
//                :GeneralProduct<::discreteGermGrain::LargeDenseIntMatrix, ::discreteGermGrain::TransitionMatrix, 5>::Base(lhs, rhs)
//         {}
}
#endif
