#include <StandardTypes.hpp>

// NOTE: STM32 toolchain has a strnlen implementation for gnu++14
//       but I haven't yet found a way of enable it.
//
//       Use the solution implemented on VxWorks by T. Canham
#ifdef TGT_OS_TYPE_THREADX
NATIVE_INT_TYPE strnlen(const char *s, NATIVE_INT_TYPE maxlen) {
    // walk buffer looking for NULL
    for (NATIVE_INT_TYPE index = 0; index < maxlen; index++) {
        if (0 == s[index]) {
            return index+1;
        }
    }
    return maxlen;
}
#endif

