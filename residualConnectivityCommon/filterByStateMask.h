#ifndef RESIDUAL_CONNECTIVITY_HEADER_GUARD
#define RESIDUAL_CONNECTIVITY_HEADER_GUARD
namespace residualConnectivity
{
	struct filterByStateMask
	{
	public:
		filterByStateMask()
			:states(NULL), mask(-1)
		{}
		filterByStateMask(vertexState* states, int mask)
			:states(states), mask(mask)
		{}
		filterByStateMask(const filterByStateMask& other)
			:states(other.states), mask(other.mask)
		{}
		template<typename vertexType> bool operator()(vertexType vertex) const
		{
			return ((states[vertex].state & mask) != 0);
		}
		filterByStateMask& operator=(const filterByStateMask& other)
		{
			states = other.states;
			mask = other.mask;
			return *this;
		}
		vertexState* states;
		int mask;
	};
}
#endif
