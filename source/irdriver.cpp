#include "irdriver.hpp"
#include "dev13.hpp"

extern "C" {
    #include "i2c.h"
}

namespace ir {
namespace driver {

Result IrDev13::Initialize() {
    if (mInitialized) {
        return 0xD8210FF9;
    }

    i2cIrInit();

    mInitialized = 1;
    mSleep = 0;

    /* Explicitly disable sleep mode for now */
    WriteIER(ReadIER());

    SetupCommunication();

    return 0;
}

uint8_t IrDev13::ReadIER() const {
    uint8_t buf = 0;
    I2C_ReadRegister8(DEV_IR, IR_REG_IER, &buf);

    return buf;
}

void IrDev13::WriteIER(const uint8_t IER) const {
    uint8_t finalVal = IER;
    if (mSleep) {
        finalVal |= IR_REG_IER_SLEEP;
    } else {
        finalVal &= ~IR_REG_IER_SLEEP;
    }

    I2C_WriteRegister8(DEV_IR, IR_REG_IER, finalVal);
}


uint16_t IRDev13::GetDivisor() const {
    /* read original LCR Value */
    uint8_t origLcrValue = 0;
    I2C_ReadRegister8(DEV_IR, IR_REC_LCR, &origLcrValue);

    /* now latch */
    I2C_WriteRegister8(DEV_IR, IR_REG_LCR, origLcrValue | IR_REG_LCR_DIV_LAT_EN);

    
}

void IRDev13::SetDivisor(const uint16_t divisor) const {

}

Result IrDev13::SetupCommunication() const {
    if (!mInitialized) {
        return 0xD8210FF8;
    }

    constinit static uint8_t lcrRegVal = 0xFF & ~IR_REG_LCR_BREAK_CTL;

    Result res = I2C_WriteRegister8(DEV_IR, IR_REG_LCR, lcrRegVal);
    if (R_FAILED(res))
        return res;

    res = I2C_WriteRegister8(DEV_IR, IR_REG_EFR, IR_REG_EFR_ENHAN_EN);
    if (R_FAILED(res))
        return res;

    const uint8_t newLcrReg = mWordLen | IR_REG_LCR_EN_PARITY(mEnableParity) \
                                | IR_REG_LCR_EN_STOP(mStopBit);

    res = I2C_WriteRegister8(DEV_IR, IR_REG_LCR, newLcrReg);
    if (R_FAILED(res))
        return res;

    const uint8_t mcrRegVal = IR_REG_MCR_TCRTLR_EN | IR_REG_MCR_SET_MODE(mMode);
    res = I2C_WriteRegister8(DEV_IR, IR_REG_MCR, mcrRegVal);
    if (R_FAILED(res))
        return res;

    return res;
}

}
} // End of namespace ir
