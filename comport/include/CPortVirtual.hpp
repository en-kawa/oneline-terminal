#ifndef C_PORT_VIRTUAL_HPP
#define C_PORT_VIRTUAL_HPP

#include "MyPortDefine.hpp"


namespace MY_PORT
{
	class PortVirtual
	{
	public:		
		typedef std::function<void(STATUS_CODE)> STATUS_FUNCTION;
		typedef std::function<void(const void *, long)> DATA_FUNCTION;
		
		virtual bool Connect(const char *file_pathname) = 0;
		virtual void Disconnect(void) = 0;
		virtual long Send(const void *buf, long size) = 0;
		virtual bool IsConnected(void) = 0;
		
		virtual void StatusCallbackSet(STATUS_FUNCTION callback)
		{
			status_callback_ = callback;
		}
		virtual void DataCallbackSet(DATA_FUNCTION callback)
		{
			data_callback_ = callback;
		}
		
	protected:
		STATUS_FUNCTION status_callback_;
		DATA_FUNCTION data_callback_;
		
	};
};


#endif
