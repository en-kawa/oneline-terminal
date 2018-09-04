#ifndef C_COMPORT_THREAD_LOG_HPP
#define C_COMPORT_THREAD_LOG_HPP

#include "CPortLogBind.hpp"
#include "CComportThread.hpp"
#include "CBinaryLogVirtual.hpp"

namespace MY_PORT
{
	typedef PortLogBind<ComportThread, BinaryLogVirtual> ComportThreadLog;
};



#endif
