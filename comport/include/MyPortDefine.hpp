#ifndef MY_PORT_DEFINE_HPP
#define MY_PORT_DEFINE_HPP

#include <functional>

namespace MY_PORT
{
	typedef enum {
		STATUS_CONNECT,
		STATUS_DISCONNECT,
	} STATUS_CODE;
	
	typedef std::function<void(const void *, long)> DATA_FUNCTION;
	typedef std::function<void(STATUS_CODE)> STATUS_FUNCTION;
};

#endif
