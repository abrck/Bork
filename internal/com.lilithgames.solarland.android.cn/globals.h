#ifndef GLOBALS_H
#define GLOBALS_H

#include <jni.h>
#include <thread>

#include "memory.h"
#include "log.h"

namespace Globals
{
    inline uintptr_t libUE4 = 0;
    inline int ScreenWidth = 0;
    inline int ScreenHeight = 0;
}


#endif // GLOBALS_H