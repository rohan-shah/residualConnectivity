#ifndef OUTPUT_OBJECT_HEADER_GUARD
#define OUTPUT_OBJECT_HEADER_GUARD
#include <iostream>
#include <string>
namespace residualConnectivity
{
	struct outputObject
	{
	public:
		virtual outputObject& operator<<(int)=0;
		virtual outputObject& operator<<(std::string)=0;
		virtual outputObject& operator<<(std::size_t)=0;
		virtual outputObject& operator<<(long)=0;
		virtual outputObject& operator<<(const char*)=0;
		typedef outputObject& (*manipulatorType)(outputObject&);
		static outputObject& endl(outputObject& obj)
		{
			return obj.internalEndl();
		}
		static outputObject& flush(outputObject& obj)
		{
			return obj.internalFlush();
		}
		outputObject& operator<<(manipulatorType manipulator)
		{
			return manipulator(*this);
		}
	protected:
		outputObject();
		virtual outputObject& internalEndl()=0;
		virtual outputObject& internalFlush()=0;
	};
	struct commandLineOutput : public outputObject
	{
	public:
		commandLineOutput()
		{}
		virtual outputObject& operator<<(int i)
		{
			std::cout << i;
			return *this;
		}
		virtual outputObject& operator<<(std::string s)
		{
			std::cout << s;
			return *this;
		}
		virtual outputObject& operator<<(std::size_t i)
		{
			std::cout << i;
			return *this;
		}
		virtual outputObject& operator<<(long i)
		{
			std::cout << i;
			return *this;
		}
		virtual outputObject& operator<<(const char* c)
		{
			std::cout << c;
			return *this;
		}
	protected:
		virtual outputObject& internalEndl()
		{
			std::cout << std::endl;
			return *this;
		}
		virtual outputObject& internalFlush()
		{
			std::cout << std::flush;
			return *this;
		}
	};
}
#endif
