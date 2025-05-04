#include "dev17driver.hpp"
#include "dev17.hpp"
#include "../utils.hpp"

extern "C" {
    #include "../i2c.h"
    #include "../gpio.h"
}

namespace ir {
namespace driver {

/* I do not understand how driver 17 works atm,
   This follows exactly what the official ir module does.
*/
Result IrDev17::Initialize() {
    if (mState > State::ReadyToInit) {
        return 0xE0A04FEA;
    }

    Result ret = 0;

    RETURN_ON_FAIL(GPIOIR_SetRegPart1(512, 512))
    RETURN_ON_FAIL(GPIOIR_SetData(512, 512))

    RETURN_ON_FAIL(I2C_WriteCommand8(DEV_CPP, IR_CMD_UNK_xC1))

    /* 0x51 and 0x52 are related and they seem to be a "mode" setting? */
    RETURN_ON_FAIL(I2C_WriteCommand8(DEV_CPP, IR_CMD_UNK_x52))

    RETURN_ON_FAIL(I2C_WriteCommand8(DEV_CPP, IR_CMD_ENABLE_POLLING))

    /* High possibilty of it being something like device variant/vendor/etc. */
    RETURN_ON_FAIL(I2C_WriteCommand8(DEV_CPP, IR_CMD_UNK_xF5))

    svcSleepThread(2000000LL);

    uint8_t buf[5];
    RETURN_ON_FAIL(I2C_ReadRegisterRaw(DEV_CPP, buf, 5))

    if (buf[4] != 0xFF || buf[0] != 0x82) {
        ret = 0xC8C04FF4;
    }

    RETURN_ON_FAIL(ret)

    if (buf[1] == 0x1) {
        mVariant = buf[2];
    }

    if (mVariant) {

        RETURN_ON_FAIL(I2C_WriteCommand8(DEV_CPP, IR_CMD_UNK_x08))

        RETURN_ON_FAIL(I2C_WriteCommand8(DEV_CPP, IR_CMD_UNK_xF3))

        Options options {1, 8, 5, 7, 21, 9};

        RETURN_ON_FAIL(SetOptions(&options))

        if (mVariant >= 2) {
            RETURN_ON_FAIL(I2C_WriteCommand8(DEV_CPP, IR_CMD_UNK_x33))
        }
    }

    return Sleep(0);
}

Result IrDev17::Sleep(const int a2) {
    if (mState == State::Sleep) {
        return 0xE0A04FEA;
    }

    Result ret = 0;

    if (a2 && mVariant == 2) {
        RETURN_ON_FAIL(I2C_WriteCommand8(DEV_CPP, IR_CMD_UNK_x37))
    }

    RETURN_ON_FAIL(I2C_WriteCommand8(DEV_CPP, IR_CMD_ENABLE_POLLING))

    svcSleepThread(6000000LL);

    /* IR module calls this, but the value is never used, why? */
    uint8_t buf[5];
    RETURN_ON_FAIL(I2C_ReadRegisterRaw(DEV_CPP, buf, 5))

    RETURN_ON_FAIL(I2C_WriteCommand8(DEV_CPP, IR_CMD_UNK_x7C))

    const uint8_t cmdid = a2 ? IR_CMD_UNK_xD0 : IR_CMD_UNK_xD2;

    RETURN_ON_FAIL(I2C_WriteCommand8(DEV_CPP, cmdid))

    mState = State::Sleep;
    mWasCmdxD0 = a2;
    mPoll = 0;
    return 0;
}

Result IrDev17::Wakeup(const int enablePolling) {
    if (mState == State::Awake) {
        return 0xE0A04FEA;
    }

    Result ret = 0;
    uint8_t wasCmdxD0 = mWasCmdxD0;

    RETURN_ON_FAIL(GPIOIR_SetRegPart1(512, 512))
    RETURN_ON_FAIL(GPIOIR_SetData(512, 512))

    svcSleepThread(100000LL);

    RETURN_ON_FAIL(GPIOIR_SetRegPart1(512, 512))
    RETURN_ON_FAIL(GPIOIR_SetData(0, 512))

    svcSleepThread(10000000LL);

    RETURN_ON_FAIL(I2C_WriteCommand8(DEV_CPP, IR_CMD_UNK_x7F))

    mPoll = enablePolling;

    /* This makes it look like xD0 does extra steps internally and we need to wait 
       for that to finish */
    if (wasCmdxD0) {
        svcSleepThread(136000000LL);
        mWasCmdxD0 = 0;
    } else {
        svcSleepThread(29000000LL);
    }

    const uint8_t cmdid = enablePolling ? IR_CMD_ENABLE_POLLING : IR_CMD_DISABLE_POLLING;

    RETURN_ON_FAIL(I2C_WriteCommand8(DEV_CPP, cmdid))

    if (wasCmdxD0 && mVariant == 2) {
        RETURN_ON_FAIL(I2C_WriteCommand8(DEV_CPP, IR_CMD_UNK_x33))
    }

    mState = State::Awake;
    return 0;
}

Result IrDev17::SetOptions(const Options *options) {
    if (!mVariant) {
        return 0xC8C04FF4;
    }

    Result ret = 0;

    if (options->unk1) {
        const uint8_t cmdid = options->unk2 ? IR_CMD_UNK_xF2 : IR_CMD_UNK_xF1;
        ret = I2C_WriteCommand8(DEV_CPP, cmdid);
    } else {
        ret = I2C_WriteCommand8(DEV_CPP, IR_CMD_UNK_xF0);
    }

    RETURN_ON_FAIL(ret)

    {
        uint8_t cmdid = 0;
        switch (options->unk2) {
            case 5:
                cmdid = 0x20;
                break;
            case 6:
                cmdid = 0x21;
                break;
            case 7:
                cmdid = 0x22;
                break;
            case 8:
                cmdid = 0x23;
                break;
            case 9:
                cmdid = 0x24;
                break;
            case 10:
                cmdid = 0x25;
                break;
            case 15:
                cmdid = 0x26;
                break;
            case 20:
                cmdid = 0x27;
                break;
            case 25:
                cmdid = 0x28;
                break;
            case 30:
                cmdid = 0x29;
                break;
            case 40:
                cmdid = 0x2A;
                break;
            case 60:
                cmdid = 0x2B;
                break;
            case 80:
                cmdid = 0x2C;
                break;
            case 100:
                cmdid = 0x2D;
                break;
            case 120:
                cmdid = 0x2E;
                break;
            default:
                cmdid = 0x2F;
                break;
        }

        RETURN_ON_FAIL(I2C_WriteCommand8(DEV_CPP, cmdid))
    }

    {
        uint8_t cmdid = 0;
        if (options->unk3 - 1 > 0xF) {
            cmdid = IR_CMD_UNK_x84;
        } else {
            cmdid = IR_CMD_UNK_x80 | ((options->unk3 - 1) & 0xF);
        }

        RETURN_ON_FAIL(I2C_WriteCommand8(DEV_CPP, cmdid))
    }

    {
        uint8_t cmdid = 0;
        if (options->unk4 - 2 <= 0xE) {
            cmdid = IR_CMD_UNK_x90 | ((options->unk4 - 1) & 0xF);
        } else {
            cmdid = IR_CMD_UNK_x96;
        }

        RETURN_ON_FAIL(I2C_WriteCommand8(DEV_CPP, cmdid))
    }

    /* TODO: Add more here */

    return 0;
}

} // End of namespace driver
} // End of namespace ir
