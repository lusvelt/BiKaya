#ifdef TARGET_UMPS
#include "umps.h"
#elif TARGET_UARM
#include "uarm.h"
#else
#error "Unsupported architecture"
#endif
