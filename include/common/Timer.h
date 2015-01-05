#pragma once

#include "common/common.h"
#include <boost/timer/timer.hpp>


// Microsecond Resolution Timer
class Timer
{
public:
    Timer()
    {
        m_timer.start();
    }
    void Reset()
    {
        m_timer.stop();
        m_timer.start();
    }

    // returns time in microseconds since last call to Reset()
    unsigned int Elapsed()
    {
        return static_cast<unsigned int>(m_timer.elapsed().wall/boost::timer::nanosecond_type(1000L));
    }
    ~Timer()
    {
        m_timer.stop();
    }
private:
    boost::timer::cpu_timer m_timer;
};