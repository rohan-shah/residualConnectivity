#ifndef RPACKAGE_R_OUTPUT_OBJECT
#define RPACKAGE_R_OUTPUT_OBJECT
#include "outputObject.h"
#include "Rcpp.h"
struct ROutputObject : public discreteGermGrain::outputObject
{
public:
	virtual outputObject& operator<<(int i)
	{
		Rcpp::Rcout << i;
		return *this;
	}
	virtual outputObject& operator<<(std::string s)
	{
		Rcpp::Rcout << s;
		return *this;
	}
	virtual outputObject& operator<<(std::size_t i)
	{
		Rcpp::Rcout << i;
		return *this;
	}
	virtual outputObject& operator<<(long i)
	{
		Rcpp::Rcout << i;
		return *this;
	}
	virtual outputObject& operator<<(const char* c)
	{
		Rcpp::Rcout << c;
		return *this;
	}
	virtual outputObject& internalEndl()
	{
		Rcpp::Rcout << std::endl;
		return *this;
	}
	virtual outputObject& internalFlush()
	{
		Rcpp::Rcout.flush();
		return *this;
	}
};
#endif
