#include "opentimeconfig.h"

#ifdef UNIX

#include "TrPOSIX.h"
#include <iostream>

Timer::TrPOSIX::~TrPOSIX() 
{
	this->onStop();
}

void Timer::TrPOSIX::onStart(unsigned int delay)
{
    /*
    The <signal.h> header defines the sigevent structure, which has at least the following members:

    int                      sigev_notify            notification type
    int                      sigev_signo             signal number
    union sigval             sigev_value             signal value
    void(*)(union sigval)    sigev_notify_function   notification function
    (pthread_attr_t*)        sigev_notify_attributes notification attributes

    MUST SET ALL VALUES OR THE TIMER WILL NOT WORK!!!!
    */
    this->evt.sigev_signo = 99999999;
    this->evt.sigev_value.sival_ptr =  (void*)this;
    this->evt.sigev_notify_function =  TrPOSIX::timerCallback;
    this->evt.sigev_notify_attributes = NULL;

    /*
    The following values of sigev_notify are defined:
    SIGEV_NONE
        No asynchronous notification will be delivered when the event of interest occurs.
    SIGEV_SIGNAL
        A queued signal, with an application-defined value, will be generated when the event of interest occurs.
    SIGEV_THREAD
    		A notification function will be called to perform notification.
    */
    this->evt.sigev_notify = SIGEV_THREAD;

    /* int timer_create(clockid_t clockid, struct sigevent *evp,  timer_t *timerid); */
    /*
    The values that clockid_t currently supports for POSIX.1b timers, as defined in include/linux.time.h, are:

    CLOCK_REALTIME
        Systemwide realtime clock.
    CLOCK_MONOTONIC
        Represents monotonic time. Cannot be set.
    CLOCK_PROCESS_CPUTIME_ID
        High resolution per-process timer.
    CLOCK_THREAD_CPUTIME_ID
        Thread-specific timer.
    CLOCK_REALTIME_HR
        High resolution version of CLOCK_REALTIME.
    CLOCK_MONOTONIC_HR
        High resolution version of CLOCK_MONOTONIC.
    */

    int res = timer_create(CLOCK_REALTIME, &this->evt, &this->mytimer);
    if (res != 0) {
        //printf("Error in timer_create!\n");
        return;
    }

    /*
    The <time.h> header declares the structure timespec, which has at least the following members:
    time_t  tv_sec    seconds
    long    tv_nsec   nanoseconds

    This header (time.h) also declares the itimerspec structure, which has at least the following members:
    struct timespec  it_interval  timer period
    struct timespec  it_value     timer expiration
    */

    // milliseconds is 10(-3) seconds, nanoseconds are 10(-9) seconds
    long int seconds = delay / 1000;
    long int nanos = (delay-(seconds*1000)) * 1000000;

    this->timer_spec.it_value.tv_sec  = seconds;
    this->timer_spec.it_value.tv_nsec = nanos;

    /* The reload value of the timer is set to the value specified by the it_interval member of value. 
    When a timer is armed with a non-zero it_interval, a periodic (or repetitive) timer is specified. */
    this->timer_spec.it_interval.tv_sec = seconds;
    this->timer_spec.it_interval.tv_nsec = nanos;

    /* long sys_timer_settime (timer_t timer_id, int flags, const struct itimerspec *new_setting, struct itimerspec *old_setting); */
    res = timer_settime(this->mytimer, 0, &this->timer_spec, NULL);
    if (res != 0) {
        //printf("Error in timer_settime!\n");
        return;
    }
}

void Timer::TrPOSIX::onStop()
{
    /* int timer_delete(timer_t timerid); */
    if (this->is_active)
    {
        timer_delete(this->mytimer);
        //std::cout << "Timer deleted!!\n";
    }
   /*
    else
      std::cout << "TrPOSIX::onStop called but timer NOT deleted!!\n";
   */
}


/*
The sigval union is defined as:

int    sival_int    integer signal value
void*  sival_ptr    pointer signal value
*/

// void(*)(union sigval)    sigev_notify_function   notification function
void Timer::TrPOSIX::timerCallback(union sigval _data)
{
    TrPOSIX* mainclass = (TrPOSIX*) _data.sival_ptr;
    mainclass->callFunc();
}

#endif
