#ifndef UNIQUE_ARRAY_HEADER_GUARD
#define UNIQUE_ARRAY_HEADER_GUARD
#include <memory>
namespace discreteGermGrain
{
	template<typename T> struct array_deleter
	{
		void operator()(T* ptr)
		{
			delete[] ptr;
		}
	};
	typedef std::unique_ptr<float, array_deleter<float> > unique_float_array;
	typedef std::unique_ptr<int, array_deleter<int> > unique_int_array;
	typedef std::unique_ptr<bool, array_deleter<bool> > unique_bool_array;
	
	template <typename T> struct unique_array2 : protected std::unique_ptr<T, array_deleter<T> > 
	{
	public:
		unique_array2(size_t size_x, size_t size_y)
			:size_x(size_x), size_y(size_y), std::unique_ptr<T, array_deleter<T> >(new T[size_x * size_y])
		{
		}
		T& operator()(size_t x, size_t y)
		{
			return this->get()[x * size_x + y];
		}
	private:
		size_t size_x, size_y;
	};
	
	typedef unique_array2<int> unique_int_array2;
	typedef unique_array2<float> unique_float_array2;
}
#endif