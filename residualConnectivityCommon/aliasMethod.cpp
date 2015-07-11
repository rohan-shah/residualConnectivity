#include "aliasMethod.h"
#include <stdexcept>
#include <boost/math/special_functions/fpclassify.hpp>
namespace aliasMethod
{
	aliasMethod::aliasMethod(std::vector<double>& probabilities, double sum, std::vector<std::ptrdiff_t>& tooBig, std::vector<std::ptrdiff_t>& tooSmall, std::vector<std::pair<double, std::ptrdiff_t> >& data) : data(data)
	{
		if(sum != sum || boost::math::isinf(sum)) throw std::runtime_error("Input sum was NA or Inf in aliasMethod");
		data.clear();
		this->sum = sum;
		std::size_t n = probabilities.size();
		tooBig.clear();
		tooSmall.clear();
		tooBig.reserve(probabilities.size());
		tooSmall.reserve(probabilities.size());
		for (std::vector<double>::iterator i = probabilities.begin(); i != probabilities.end(); i++)
		{
			*i *= n;
		}
		for (std::vector<double>::iterator i = probabilities.begin(); i != probabilities.end(); i++)
		{
			if (*i < sum)
			{
				tooSmall.push_back(std::distance(probabilities.begin(), i));
			}
			else tooBig.push_back(std::distance(probabilities.begin(), i));
		}
		data.resize(probabilities.size());
		while (tooSmall.size() > 0 && tooBig.size() > 0)
		{
			//double tmp = 0;
			//for (std::vector<double>::iterator i = probabilities.begin(); i != probabilities.end(); i++) tmp += *i;
			//tmp /= sum;
			std::ptrdiff_t tooBigIndex = *tooBig.rbegin(), tooSmallIndex = *tooSmall.rbegin();
			data[tooSmallIndex].first = probabilities[tooSmallIndex];
			data[tooSmallIndex].second = tooBigIndex;
			probabilities[tooBigIndex] -= sum - probabilities[tooSmallIndex];
			tooSmall.pop_back();
			if (probabilities[*tooBig.rbegin()] < sum)
			{
				tooSmall.push_back(*tooBig.rbegin());
				tooBig.pop_back();
			}
		}
		if(tooSmall.size() == 0 && tooBig.size() > 0)
		{
			for(std::vector<std::ptrdiff_t>::iterator i = tooBig.begin(); i != tooBig.end(); i++)
			{
				std::ptrdiff_t index = *i;
				data[index].first = sum;
				data[index].second = -1;
			}
			tooBig.clear();
		}
		else if(tooBig.size() == 0 && tooSmall.size() > 0)
		{
			for(std::vector<std::ptrdiff_t>::iterator i = tooSmall.begin(); i != tooSmall.end(); i++)
			{
				std::ptrdiff_t index = *i;
				data[index].first = sum;
				data[index].second = -1;
			}
			tooSmall.clear();
		}
		if (tooSmall.size() + tooBig.size() == 1)
		{
			std::ptrdiff_t index;
			if (tooSmall.size() == 1)
			{
				index = tooSmall[0];
			}
			else
			{
				index = tooBig[0];
			}
			data[index].first = sum;
			data[index].second = -1;
		}
		else if (tooSmall.size() + tooBig.size() > 1)
		{
			throw std::runtime_error("Internal error");
		}
	}
}
