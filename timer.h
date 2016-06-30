/*
 * timer.h
 *
 *  Created on: May 7, 2015
 *      Author: rashid
 */

#ifndef GALOISGPU_GOPT_TIMER_H_
#define GALOISGPU_GOPT_TIMER_H_


#if _GALOIS_HETERO_DISABLE_TIMER
/////////Disable timers.
struct Timer {
   Timer() {
   }
   void clear() {
   }
   void start() {
   }
   void stop() {
   }
   double get_time_seconds(void) {
      return 0;
   }
};//End struct timer;
#else

#ifdef _WIN32
/////////Windows timers
struct Timer {
   double _start;
   double _end;
   Timer() :
         _start(0), _end(0) {
   }
   void clear() {
      _start = _end = 0;
   }
   void start() {
      _start = rtclock();
   }
   void stop() {
      _end = rtclock();
   }
   double get_time_seconds(void) {
      return (_end - _start);
   }
   static double rtclock() {
      LARGE_INTEGER tickPerSecond, tick;
      QueryPerformanceFrequency(&tickPerSecond);
      QueryPerformanceCounter(&tick);
      return (tick.QuadPart*1000000/tickPerSecond.QuadPart)*1.0e-6;
      //return (Tp.tv_sec + Tp.tv_usec * 1.0e-6);
   }
};//End struct timer;
#else
#ifdef  HAVE_CXX11_CHRONO
//#include <ctime>
//#include <ratio>
#include <chrono>
   struct Timer {
   using namespace std::chrono;
   double _start, _end;
      Timer():_start(0), _end(0) {
      }
      void clear() {
         _start = _end = 0;
      }
      void start() {
         _start = rtclock();
      }
      void stop() {
         _end = rtclock();
      }
      double get_time_seconds(void) {
         using namespace std::chrono;
         return (_end - _start)/system_clock::period::den;
      }
      static  double rtclock() {
         using namespace std::chrono;
         return  std::chrono::high_resolution_clock::now().time_since_epoch().count();
      }
};
#else
   struct Timer {
      double _start;
      double _end;
      Timer() :
            _start(0), _end(0) {
      }
      void clear() {
         _start = _end = 0;
      }
      void start() {
         _start = rtclock();
      }
      void stop() {
         _end = rtclock();
      }
      double get_time_seconds(void) {
         return (_end - _start);
      }
   #include <time.h>
      static double rtclock() {
         struct timespec t;
         clock_gettime(CLOCK_MONOTONIC_RAW, &t);
         return (t.tv_sec + t.tv_nsec * 1.0e-9);
      }
   };
#endif //End if 0

#endif //End if WINDOWS

#endif //End if disable timers.

#endif /* GALOISGPU_GOPT_TIMER_H_ */
