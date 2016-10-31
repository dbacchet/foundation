#pragma once
// High resolution timer
// 2012-04-11, Davide Bacchet (davide.bacchet@gmail.com)
//            * changed the class name, to blend into GL utils

#ifdef _WIN32   // Windows system specific
#include <windows.h>
#else          // Unix based system specific
#include <sys/time.h>
#include <unistd.h>
#endif

namespace utils
{
    /**
    * @brief sleep in milli seconds
    * @param in_mseconds the number of milli seconds
    */
    inline void sleep( unsigned int in_mseconds ){
      #ifdef _WIN32
        Sleep( in_mseconds );
      #else
        usleep( in_mseconds * 1000 );
      #endif
    }
    /**
    * @brief sleep in seconds
    * @param in_seconds the number of seconds
    */
    inline void ssleep( unsigned int in_seconds ){
      #ifdef _WIN32
        Sleep(in_seconds * 1000);
      #else
        sleep(in_seconds);
      #endif
    }



    /// \class HPTimer
    /// \brief high precision timer
    class HPTimer
    {
    public:
        HPTimer();                                  // default constructor
        ~HPTimer();                                 // default destructor

        void   start();                             // start timer
        void   stop();                              // stop the timer
        double get_elapsed_time();                  // get elapsed time in second
        double get_elapsed_time_in_s();           // get elapsed time in second (same as get_elapsed_time)
        double get_elapsed_time_in_ms();      // get elapsed time in milli-second
        double get_elapsed_time_in_us();      // get elapsed time in micro-second


    private:
        double startTimeInMicroSec;                 // starting time in micro-second
        double endTimeInMicroSec;                   // ending time in micro-second
        int    stopped;                             // stop flag 
    #ifdef _WIN32
        LARGE_INTEGER frequency;                    // ticks per second
        LARGE_INTEGER startCount;                   //
        LARGE_INTEGER endCount;                     //
    #else
        timeval startCount;                         //
        timeval endCount;                           //
    #endif
    };



    ///////////////////////////////////////////////////////////////////////////////
    //  class implementation
    ///////////////////////////////////////////////////////////////////////////////
    inline HPTimer::HPTimer()
    {
    #ifdef _WIN32
        QueryPerformanceFrequency(&frequency);
        startCount.QuadPart = 0;
        endCount.QuadPart = 0;
    #else
        startCount.tv_sec = startCount.tv_usec = 0;
        endCount.tv_sec = endCount.tv_usec = 0;
    #endif

        stopped = 0;
        startTimeInMicroSec = 0;
        endTimeInMicroSec = 0;
    }

    inline HPTimer::~HPTimer()
    {
    }

    inline void HPTimer::start()
    {
        stopped = 0; // reset stop flag
    #ifdef _WIN32
        QueryPerformanceCounter(&startCount);
    #else
        gettimeofday(&startCount, NULL);
    #endif
    }

    inline void HPTimer::stop()
    {
        stopped = 1; // set timer stopped flag

    #ifdef _WIN32
        QueryPerformanceCounter(&endCount);
    #else
        gettimeofday(&endCount, NULL);
    #endif
    }

    inline double HPTimer::get_elapsed_time_in_us()
    {
    #ifdef _WIN32
        if(!stopped)
            QueryPerformanceCounter(&endCount);

        startTimeInMicroSec = startCount.QuadPart * (1000000.0 / frequency.QuadPart);
        endTimeInMicroSec = endCount.QuadPart * (1000000.0 / frequency.QuadPart);
    #else
        if(!stopped)
            gettimeofday(&endCount, NULL);

        startTimeInMicroSec = (startCount.tv_sec * 1000000.0) + startCount.tv_usec;
        endTimeInMicroSec = (endCount.tv_sec * 1000000.0) + endCount.tv_usec;
    #endif

        return endTimeInMicroSec - startTimeInMicroSec;
    }

    inline double HPTimer::get_elapsed_time_in_ms()
    {
        return this->get_elapsed_time_in_us() * 0.001;
    }

    inline double HPTimer::get_elapsed_time_in_s()
    {
        return this->get_elapsed_time_in_us() * 0.000001;
    }

    inline double HPTimer::get_elapsed_time()
    {
        return this->get_elapsed_time_in_s();
    }

}

