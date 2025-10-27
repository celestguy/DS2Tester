//
// Created by theo on 27/10/2025.
//

#include "pad.h"

#include <kernel.h>
#include <loadfile.h>
#include <stdio.h>

#include "libpad.h"

static char actAlign[6];
static int actuators;

void loadModules()
{
    int ret = SifLoadModule("rom0:SIO2MAN", 0, NULL);
    if (ret < 0) {
        printf("sifLoadModule sio failed: %d\n", ret);
        SleepThread();
    }

    ret = SifLoadModule("rom0:PADMAN", 0, NULL);
    if (ret < 0) {
        printf("sifLoadModule pad failed: %d\n", ret);
        SleepThread();
    }
}

/*
 * waitPadReady()
 */
void waitPadReady()
{
    char stateString[16];

    int state = padGetState(PAD_DEFAULT_PORT, PAD_DEFAULT_SLOT);
    int lastState = -1;

    while((state != PAD_STATE_STABLE) && (state != PAD_STATE_FINDCTP1)) {
        if (state != lastState) {
            padStateInt2String(state, stateString);
            printf("Please wait, pad(%d,%d) is in state %s\n",
                       PAD_DEFAULT_PORT, PAD_DEFAULT_SLOT, stateString);
        }
        lastState = state;
        state=padGetState(PAD_DEFAULT_PORT, PAD_DEFAULT_SLOT);
    }

    // Were the pad ever 'out of sync'?
    if (lastState != -1) {
        printf("Pad OK!\n");
    }
}


/*
 * initializePad()
 */
void initPad()
{
    int ret;
    int i;

    waitPadReady();

    // How many different modes can this device operate in?
    // i.e. get # entrys in the modetable
    const int modes = padInfoMode(PAD_DEFAULT_PORT, PAD_DEFAULT_SLOT, PAD_MODETABLE, -1);
    printf("The device has %d modes\n", modes);

    if (modes > 0) {
        printf("( ");
        for (i = 0; i < modes; i++) {
            printf("%d ", padInfoMode(PAD_DEFAULT_PORT, PAD_DEFAULT_SLOT, PAD_MODETABLE, i));
        }
        printf(")");
    }

    printf("It is currently using mode %d\n",
               padInfoMode(PAD_DEFAULT_PORT, PAD_DEFAULT_SLOT, PAD_MODECURID, 0));

    // If modes == 0, this is not a Dual shock controller
    // (it has no actuator engines)
    if (modes == 0) {
        printf("This is a digital controller?\n");
        return;
    }

    // Verify that the controller has a DUAL SHOCK mode
    i = 0;
    do {
        if (padInfoMode(PAD_DEFAULT_PORT, PAD_DEFAULT_SLOT, PAD_MODETABLE, i) == PAD_TYPE_DUALSHOCK)
            break;
        i++;
    } while (i < modes);
    if (i >= modes) {
        printf("This is no Dual Shock controller\n");
        return;
    }

    // If ExId != 0x0 => This controller has actuator engines
    // This check should always pass if the Dual Shock test above passed
    ret = padInfoMode(PAD_DEFAULT_PORT, PAD_DEFAULT_SLOT, PAD_MODECUREXID, 0);
    if (ret == 0) {
        printf("This is no Dual Shock controller??\n");
        return;
    }

    printf("Enabling dual shock functions\n");

    // When using MMODE_LOCK, user cant change mode with Select button
    padSetMainMode(PAD_DEFAULT_PORT, PAD_DEFAULT_SLOT, PAD_MMODE_DUALSHOCK, PAD_MMODE_LOCK);

    waitPadReady();
    printf("infoPressMode: %d\n", padInfoPressMode(PAD_DEFAULT_PORT, PAD_DEFAULT_SLOT));

    waitPadReady();
    printf("enterPressMode: %d\n", padEnterPressMode(PAD_DEFAULT_PORT, PAD_DEFAULT_SLOT));

    waitPadReady();
    actuators = padInfoAct(PAD_DEFAULT_PORT, PAD_DEFAULT_SLOT, -1, 0);
    printf("# of actuators: %d\n",actuators);

    if (actuators != 0) {
        actAlign[0] = 0;   // Enable small engine
        actAlign[1] = 1;   // Enable big engine
        actAlign[2] = 0xff;
        actAlign[3] = 0xff;
        actAlign[4] = 0xff;
        actAlign[5] = 0xff;

        waitPadReady();
        printf("padSetActAlign: %d\n",
                   padSetActAlign(PAD_DEFAULT_PORT, PAD_DEFAULT_SLOT, actAlign));
    }
    else {
        printf("Did not find any actuators.\n");
    }

    waitPadReady();
}
