//three-bit states. These include an OFF value
#define GETSTATE3(state, count) ((state & (7ULL << (3*(count)))) >> (3*(count)))
#define SETSTATE3(state, count, newValue) (state = (state & ~(7ULL << (3*(count)))) + (((unsigned long long)newValue) << (3*(count))))
#define INCREMENTSTATE3(state, count) (state += (1ULL << (3*(count))))
//two-bit states, for situations where we DON'T want to be able to encode an OFF value. Hence the +1.  
#define GETSTATE2(state, count) (((state & (3ULL << (2 * (count)))) >> (2 * count))+1)
#define SETSTATE2(state, count, newValue) (state = (state & ~(7ULL << (2*(count)))) + (((unsigned long long)newValue-1) << (2*(count))))
