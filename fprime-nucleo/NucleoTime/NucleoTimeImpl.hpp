/*
 * TestTelemRecvImpl.hpp
 *
 *  Created on: Mar 28, 2014
 *      Author: tcanham
 */

#ifndef LINUXTIMEIMPL_HPP_
#define LINUXTIMEIMPL_HPP_

#include <Svc/Time/TimeComponentAc.hpp>

namespace Svc {

class NucleoTimeImpl: public TimeComponentBase {
    public:
        NucleoTimeImpl(const char* compName);
        virtual ~NucleoTimeImpl();
        void init(NATIVE_INT_TYPE instance);
    protected:
        void timeGetPort_handler(
                NATIVE_INT_TYPE portNum, /*!< The port number*/
                Fw::Time &time /*!< The U32 cmd argument*/
            );
    private:
};

}

#endif /* LINUXTIMEIMPL_HPP_ */
