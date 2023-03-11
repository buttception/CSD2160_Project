#ifndef _TIMER_H
#define _TIMER_H

#include <windows.h>

class _Timer
{
    private:
        bool     s_bUsingQPF = false;
        LONGLONG s_llQPFTicksPerSec = 0;
        LONGLONG s_llLastElapsedTime = 0;

    public:
        _Timer();
        ~_Timer();

    public:
        float GetTimer_sec( void );
        float GetTimer_msec( void );
};

#endif // _TIMER_H