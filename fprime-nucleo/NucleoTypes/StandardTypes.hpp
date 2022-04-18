#include <stdint.h>
#include "tx_api.h"

#define NAME_MAX 255
#define FPRIME_OVERRIDE_NATIVE_TYPES

#ifdef TGT_OS_TYPE_THREADX
typedef TX_THREAD* TaskIdRepr;
#else
typedef int TaskIdRepr;
#endif

typedef int32_t NATIVE_INT_TYPE;
typedef uint32_t NATIVE_UINT_TYPE;

#ifdef TGT_OS_TYPE_THREADX
NATIVE_INT_TYPE strnlen(const char *s, NATIVE_INT_TYPE maxlen);
#endif

