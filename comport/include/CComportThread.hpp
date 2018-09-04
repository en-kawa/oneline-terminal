#ifndef C_COMPORT_THREAD_HPP
#define C_COMPORT_THREAD_HPP

#include <thread>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <string>

#include "CPortVirtual.hpp"

namespace MY_PORT
{
	class ComportThread : public PortVirtual
	{
	public:
		typedef struct {
			speed_t speed;
			struct timeval timeout;
		} SETTING;
		
		ComportThread();
		ComportThread(const SETTING &setting);
		virtual~ComportThread();
		
		void PortSetting(const SETTING &setting);
		bool Connect(const char *file_pathname);
		void Disconnect(void);
		virtual long Send(const void *buf, long size);
		bool IsConnected(void);
		
	protected:
		void ReadThread(void);
		
		std::thread read_thread_;
		bool run_flag_;
		int fd_;					// file descriptor
		struct termios old_tio_;
		std::string file_pathname_;
		SETTING setting_;
	};
};



#endif
