#include <unistd.h>
#include <getopt.h>
#include <future>
#include <signal.h>
#include <vector>
#include <fstream>
#include <mutex>
#include <time.h>

#include "CComportThread.hpp"
#include "ColerdStream.hpp"


using namespace MY_PORT;


volatile static bool flag = true;
volatile static bool binary = false;
static bool file_flag = false;
static std::ofstream ofs;
static std::mutex mtx;


static void SigHandler(int p_signame)
{
	std::cout << std::endl << ioscc::red << "please Ctrl+D" << ioscc::reset << std::endl;
}


static void ReadCallback(std::ostream &os, const void *buf, long size)
{
	std::lock_guard<std::mutex> lock(mtx);
	if (file_flag) {
		if (binary) {
			os << "(receive binary) ";
		}
		else {
			os << "(receive text) ";
		}
	}
	else {
		os << ioscc::green;
	}
	if (binary) {
		const unsigned char *p = static_cast<const unsigned char *>(buf);
		os << std::hex;
		for (int i = 0; i < size; i++) {
			os << std::setfill('0') << std::setw(2) << std::right << static_cast<unsigned int>(p[i]) << ", ";
		}
		os << std::dec;
	}
	else {
		os.write(static_cast<const char *>(buf), size);
	}
	if (!file_flag) {
		os << ioscc::reset;
	}
	os << std::endl;
}

static void StatusCallback(STATUS_CODE code)
{
	switch (code) {
		case STATUS_CONNECT:
			std::cout << ioscc::red  << "connect" << ioscc::reset << std::endl;
			if (binary) {
				std::cout << ioscc::red << "input mode binary" << ioscc::reset << std::endl;
			}
			else {
				std::cout << ioscc::red << "input mode text" << ioscc::reset << std::endl;
			}
			break;
		case STATUS_DISCONNECT:
			flag = false;
			std::cout << ioscc::red  << "disconnect" << ioscc::reset << std::endl;
			break;
		default:
			break;
	}
}




int main (int argc, char *argv[])
{
	if (signal(SIGINT, SigHandler) == SIG_ERR) {
		std::cout << ioscc::red  << "signal set error" << ioscc::reset << std::endl;
		return 1;
	}

	binary = false;

	ComportThread::SETTING setting = {
		B9600,
		{10,0},
	};

	std::string log_file_str;

	int opt = 0;
	int option_index = 0;
	struct option long_options[] = {
		{"help", no_argument, NULL, 'h'},
		{"binary", no_argument, NULL, 'b'},
		{"rate", required_argument, NULL, 'r'},
		{"log", required_argument, NULL, 'l'},
		{0, 0, 0, 0}
	};

	while((opt = getopt_long(argc, argv, "hbr:l:", long_options, &option_index)) != -1) {
		switch (opt) {
			case 'h':
				// help (--help, -h)
				std::cout << std::endl << "This application is for device terminal control" << std::endl;
				std::cout << "please input device file (etc. [/dev/tty.usb***])" << std::endl;
				std::cout << "Press \"Ctrl + P + enter\" to change text <-> binary." << std::endl;
				std::cout << "Press \"Ctrl + D\" to exit." << std::endl;
				std::cout << std::endl << "**********[coler]**********" << std::endl;
				std::cout << "input (send)" << std::endl;
				std::cout << ioscc::green << "output (receive)" << std::endl;
				std::cout << ioscc::red << "protocol comment" << std::endl;
				std::cout << ioscc::reset;
				std::cout << std::endl << "**********[option]**********" << std::endl;
				std::cout << "-h, --help:			help" << std::endl;
				std::cout << "-b, --binary:			binary data communication" << std::endl;
				std::cout << "-r, --rate:			set serial port baudrate (etc. -r 115200, --rate=115200)" << std::endl;
				std::cout << "-l, --log:			set input output log file (etc. -l tmp.log, --log=tmp.log)" << std::endl << std::endl;
				return 0;
				break;
			case 'b':
				// binary data (--binary, -b)
				binary = true;
				break;
			case 'r':
				// baudrate set (--rate, -r)
				setting.speed = atoi(optarg);
				break;
			case 'l':
				// send receive log set (--log, -l)
				file_flag = true;
				log_file_str = optarg;
				break;
			default:
				return 0;
				break;
		}
	}

	std::string device_file;



	if (argc == optind + 1) {
		device_file = argv[optind];
	}
	else {
		std::cout << ioscc::red << "please input device file (etc. [/dev/tty.usb***])" << ioscc::reset << std::endl;
		return 0;
	}



	if (setting.speed <= 0) {
		std::cout << ioscc::red << "baudrate error" << ioscc::reset << std::endl;
		return 0;
	}

	std::ostream *os;
	if (file_flag) {
		ofs.open(log_file_str, std::ios::app);
		if (!ofs) {
			std::cout << ioscc::red << "open file(log) error" << ioscc::reset << std::endl;
			return 0;
		}
		os = static_cast<std::ostream*>(&ofs);

		struct tm tm;
		time_t t = time(NULL);
		localtime_r(&t, &tm);
		ofs << "(" << std::setfill('0') << std::setw(4) << (tm.tm_year + 1900);
		ofs << "/" << std::setfill('0') << std::setw(2) << (tm.tm_mon + 1);
		ofs << "/" << std::setfill('0') << std::setw(2) << tm.tm_mday;
		ofs << " " << std::setfill('0') << std::setw(2) << tm.tm_hour;
		ofs << ":" << std::setfill('0') << std::setw(2) << tm.tm_min;
		ofs << ":" << std::setfill('0') << std::setw(2) << tm.tm_sec;
		ofs << ")" << std::endl;
	}
	else {
		os = static_cast<std::ostream*>(&std::cout);
	}

	ComportThread port(setting);
	port.StatusCallbackSet(StatusCallback);
	port.DataCallbackSet(std::bind(ReadCallback, std::ref(*os), std::placeholders::_1, std::placeholders::_2));
	port.Connect(device_file.c_str());

	if (!port.IsConnected()) {
		std::cout << ioscc::red << "open file(tty) error" << ioscc::reset << std::endl;
		return 0;
	}


	while(true) {
		std::string str;
		std::cin >> str;
		if (!flag) {			// disconnect
			break;
		}
		if (std::cin.eof()) {	// ctrl+D
			break;
		}
		if (str[0] == 0x10) {	// ctrl+P
			binary = !binary;
			if (binary) {
				std::cout << ioscc::red << "input mode binary" << ioscc::reset << std::endl;
			}
			else {
				std::cout << ioscc::red << "input mode text" << ioscc::reset << std::endl;
			}
		}
		else {
			std::lock_guard<std::mutex> lock(mtx);
			if (binary) {
				if (str.find_first_not_of("0123456789ABCDEFabcdef, ") != std::string::npos) {
					std::cout << ioscc::red << "please input hex (etc. \"043AFF\" or \"04, 3A, FF\")" << ioscc::reset << std::endl;
				}
				else {
					for(std::string::size_type pos = str.find_first_of(", "); pos != std::string::npos; pos = str.find_first_of(", ")){
						str.erase(pos, 1);
					}

					if ((str.length() % 2) != 0) {
						std::cout << ioscc::red << "hex error" << ioscc::reset << std::endl;
					}
					else {
						std::vector<unsigned char> write;
						char buf[2 + 1];
						for (std::string::size_type i = 0; i < str.length() - 1; i += 2) {
							str.copy(buf, 2, i);
							write.push_back(std::stoi(buf, nullptr, 16));
						}
						port.Send(&(write[0]), write.size());
						if (file_flag) {
							ofs << "(send binary) " << std::hex;
							for (int i = 0; i < write.size(); i++) {
								ofs << std::setfill('0') << std::setw(2) << std::right << static_cast<unsigned int>(write[i]) << ", ";
							}
							ofs << std::dec << std::endl;
						}
					}
				}
			}
			else {
				port.Send(str.c_str(), str.length());
				if (file_flag) {
					ofs << "(send text) " << str << std::endl;
				}

			}
		}
	}
	if (file_flag) {
		std::lock_guard<std::mutex> lock(mtx);
		ofs << std::endl;
	}

	return 0;
}
