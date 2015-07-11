#ifndef ALIAS_METHOD_HEADER_GUARD
#define ALIAS_METHOD_HEADER_GUARD
#include <vector>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
namespace aliasMethod
{
	class aliasMethod
	{
	public:
		aliasMethod(std::vector<double>& probabilities, double sum, std::vector<std::ptrdiff_t>& tooBig, std::vector<std::ptrdiff_t>& tooSmall, std::vector<std::pair<double, std::ptrdiff_t> >& data);
		template<typename V> std::size_t operator()(V& randomSource)
		{
			boost::uniform_int<std::size_t> discreteDist(0, data.size() - 1);
			std::size_t index = discreteDist(randomSource);
			std::pair<double, std::ptrdiff_t>& entry = data[index];
			boost::uniform_real<double> continuousDist(0, sum);
			if (continuousDist(randomSource) < entry.first)
			{
				return index;
			}
			return entry.second;
		}
	private:
		double sum;
		//Entry at index i has the form (probability of i, index if not i)
		std::vector<std::pair<double, std::ptrdiff_t> >& data;
	};
}
#endif
