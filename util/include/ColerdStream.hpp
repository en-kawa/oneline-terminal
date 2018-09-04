#ifndef COLERD_STREAM_HPP
#define COLERD_STREAM_HPP

#include <iostream>
#include <iomanip>

namespace ioscc
{
	enum TColorCode {
		reset=1,
		red,
		blue,
		green,
	};
	
	const char *_sesc    = "\033[";
	const char *_sreset  = "0m";
	const char *_sred    = "31;1m";
	const char *_sblue   = "34;1m";
	const char *_sgreen  = "32;1m";
	
	std::ostream& operator << (std::ostream &os, ioscc::TColorCode ccode)
	{
		switch (ccode)
		{
			case ioscc::reset:
				os << ioscc::_sesc << ioscc::_sreset << std::flush;
				break;
			case ioscc::red:
				os << ioscc::_sesc << ioscc::_sred << std::flush;
				break;
			case ioscc::blue:
				os << ioscc::_sesc << ioscc::_sblue << std::flush;
				break;
			case ioscc::green:
				os << ioscc::_sesc << ioscc::_sgreen << std::flush;
				break;
		}
		return os;
	}
};




#endif
