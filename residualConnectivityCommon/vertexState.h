#ifndef VERTEX_STATE_HEADER_GUARD
#define VERTEX_STATE_HEADER_GUARD
namespace residualConnectivity
{
	enum
	{
		UNFIXED_OFF = 1, UNFIXED_ON = 2, FIXED_ON = 4, FIXED_OFF = 8,
		ON_MASK = 6, FIXED_MASK = 12, UNFIXED_MASK = 3
	};
	struct vertexState
	{
		int state;
		static vertexState fixed_on()
		{
			vertexState ret;
			ret.state = FIXED_ON;
			return ret;
		}
		static vertexState fixed_off()
		{
			vertexState ret;
			ret.state = FIXED_OFF;
			return ret;
		};
		static vertexState unfixed_off()
		{
			vertexState ret;
			ret.state = UNFIXED_OFF;
			return ret;
		};
		static vertexState unfixed_on()
		{
			vertexState ret;
			ret.state = UNFIXED_ON;
			return ret;
		}
		template<class Archive> void serialize(Archive& ar, const unsigned int version)
		{
			ar & state;
		};
	};
}
#endif
