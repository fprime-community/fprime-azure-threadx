#include "tim.h"
#include <Os/IntervalTimer.hpp>
#include <Fw/Types/Assert.hpp>


namespace Os {

    U32 IntervalTimer::getDiffUsec(const RawTime& t1, const RawTime& t2) {

        RawTime result = {t1.upper - t2.upper,0};

        if (t1.lower < t2.lower)
        {
            result.upper -= 1;
            result.lower = t1.lower + (1000000000 - t2.lower);
        }
        else
        {
            result.lower = t1.lower - t2.lower;
        }

        return result.lower;
    }

    void IntervalTimer::getRawTime(RawTime& time) {
        // Get STM32H745ZI::TIM2::CNT value
        time.lower = __HAL_TIM_GET_COUNTER(&htim2);
        time.upper = 0;
    }
}
