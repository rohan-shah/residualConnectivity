#include "calculateFactorials.h"
namespace discreteGermGrain
{
	void calculateFactorials(std::vector<mpz_class>& factorials, int upperLimit)
	{
		factorials.clear();
		factorials.reserve(upperLimit);
		factorials.push_back(1);
		factorials.push_back(1);
		for (int i = 2; i < upperLimit; i++)
		{
			factorials.push_back(factorials[i - 1] * i);
		}
	}
}