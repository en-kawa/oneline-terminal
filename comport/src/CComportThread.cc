#include "CComportThread.hpp"
#include <string.h>

#define PORT_READ_BUF_NUM	256

using namespace MY_PORT;

ComportThread::ComportThread()
: fd_(-1)
, run_flag_(false)
, file_pathname_("")
{
}

ComportThread::ComportThread(const SETTING &setting)
: fd_(-1)
, run_flag_(false)
, setting_(setting)
, file_pathname_("")
{
}

ComportThread::~ComportThread()
{
	Disconnect();
}

void ComportThread::PortSetting(const SETTING &setting)
{
	setting_ = setting;
}

bool ComportThread::Connect(const char *file_pathname)
{
	if ((read_thread_.joinable()) || (run_flag_)) {
		return false;
	}

	fd_ = open(file_pathname, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd_ < 0) {
		return false;
	}
	file_pathname_ = file_pathname;
	fcntl(fd_, F_SETFL, 0);

	tcgetattr(fd_, &old_tio_);	// old termios
	struct termios new_tio;		// setting new termios
	memset(&new_tio, 0, sizeof(struct termios));
	new_tio.c_cflag = CS8 | CLOCAL | CREAD;
	new_tio.c_iflag = IGNPAR;
	new_tio.c_oflag = 0;
	new_tio.c_lflag = 0;
	new_tio.c_cc[VMIN] = 1;
	new_tio.c_cc[VTIME] = 0;

	cfsetspeed(&new_tio, setting_.speed);
	tcflush(fd_, TCIOFLUSH);	// clean data
	tcsetattr(fd_, TCSANOW, &new_tio);

	run_flag_ = true;
	read_thread_ = std::thread(&ComportThread::ReadThread, this);
	return true;
}

void ComportThread::Disconnect(void)
{
	if (read_thread_.joinable()) {
		run_flag_ = false;
		read_thread_.join();
		fd_ = -1;
	}
}

long ComportThread::Send(const void *buf, long size)
{
	if (!IsConnected()) {
		return 0;
	}
	return write(fd_, buf, size);
}

bool ComportThread::IsConnected(void)
{
	return ((read_thread_.joinable()) && (run_flag_));
}



void ComportThread::ReadThread(void)
{
	fd_set read_fs;

	if (status_callback_) {
		status_callback_(STATUS_CONNECT);
	}

	while (run_flag_) {
		std::this_thread::sleep_for(std::chrono::milliseconds(0));	// change thread
		struct timeval timeout = setting_.timeout;
		FD_SET(fd_, &read_fs);
		if (select(fd_ + 1, &read_fs, NULL, NULL, &timeout) == 1) {
			unsigned char buf[PORT_READ_BUF_NUM];
			long len = read(fd_, buf, PORT_READ_BUF_NUM);
			if (len <= 0) {
				// error
				run_flag_ = false;
				break;
			}
			if (data_callback_) {
				data_callback_(buf, len);
			}
		}
	}

	tcsetattr(fd_, TCSANOW, &old_tio_);
	close(fd_);
	if (status_callback_) {
		status_callback_(STATUS_DISCONNECT);
	}
}
