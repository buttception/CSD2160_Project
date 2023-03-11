#include "timer.h"

_Timer::_Timer()
{
    s_bUsingQPF = false;
    s_llQPFTicksPerSec = 0;
    s_llLastElapsedTime = 0;

    LARGE_INTEGER qwTicksPerSec, qwTime;
    s_bUsingQPF = (bool)( QueryPerformanceFrequency( &qwTicksPerSec ) != 0 );

    if( !s_bUsingQPF )
        return;

    s_llQPFTicksPerSec = qwTicksPerSec.QuadPart;

    QueryPerformanceCounter( &qwTime );
    s_llLastElapsedTime = qwTime.QuadPart;
}

_Timer::~_Timer()
{
}

// Return in ms.

float _Timer::GetTimer_sec( void )
{
    if( !s_bUsingQPF )
        return  -1.0;

    LARGE_INTEGER qwTime;
    QueryPerformanceCounter( &qwTime );

    float  fElapsedTime = (float)( (double)( qwTime.QuadPart - s_llLastElapsedTime ) / (double)s_llQPFTicksPerSec );
    s_llLastElapsedTime = qwTime.QuadPart;

    return  fElapsedTime;
}

float _Timer::GetTimer_msec( void )
{
    return ( GetTimer_sec() * 1000 );
}
