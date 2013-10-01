// File: Timer.h

// Author: Yuxin Wu <ppwwyyxxc@gmail.com>

#pragma once
#ifdef WIN32   // Windows system specific
#include <windows.h>
#else          // Unix based system specific
#include <sys/time.h>
#endif


class Timer {
	public:
		Timer();
		~Timer(){}

		void   reset();
		void   stop();
		double get_time_microsec();

		inline double get_time_millisec() { return get_time_microsec() * 1e-3; }

		inline double get_time_sec() { return get_time_microsec() * 1e-6; }

		inline double get_time() { return get_time_sec(); }

	private:
		bool stopped;

#ifdef WIN32
		LARGE_INTEGER freq;
		LARGE_INTEGER startCount;
		LARGE_INTEGER endCount;
#else
		timeval startCount;
		timeval endCount;
#endif
};
