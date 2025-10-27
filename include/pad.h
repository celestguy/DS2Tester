//
// Created by theo on 27/10/2025.
//

#ifndef DS2TESTER_PAD_H
#define DS2TESTER_PAD_H

#endif //DS2TESTER_PAD_H

#include <sifrpc.h>
#include <tamtypes.h>

#include "libpad.h"

#define PAD_DEFAULT_PORT 0
#define PAD_DEFAULT_SLOT 0

void loadModules();

void initPad();

void waitPadReady();