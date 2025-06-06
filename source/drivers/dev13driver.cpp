#include "dev13driver.hpp"
#include "dev13.hpp"

extern "C" {
    #include "../i2c.h"
    #include "../gpio.h"
}

namespace ir {
namespace driver {

Result IrDev13::Initialize() {
    if (mInitialized) {
        return 0xD8210FF9;
    }

    Result res = i2cIrInit();
    if (R_FAILED(res))
        return res;

    res = gpioIrInit();
    if (R_FAILED(res))
        return res;

    InitializeGpio();

    mInitialized = 1;
    mSleep = 0;

    /* Explicitly disable sleep mode for now */
    WriteIER(ReadIER());

    res = SetupCommunication();
    if (R_FAILED(res))
        return res;

    ResetEFCR();
    IOLatchEnable();

    SetOutputPinsState(mOutputPinState);
    GoToSleep();
    SetIrLedState(0);

    return 0;
}

Result IrDev13::ResetEFCR() const {
    return I2C_WriteRegister8(DEV_IR, IR_REG_EFCR, 0);
}

Result IrDev13::IOLatchEnable() const {
    return I2C_WriteRegister8(DEV_IR, IR_REG_IOCTL, IR_REG_IOCTL_IOLATCH_EN);
}

Result IrDev13::SetOutputPinsState(const uint8_t state) const {
    return I2C_WriteRegister8(DEV_IR, IR_REG_IOSTA, state);
}

Result IrDev13::DisableTxRx() const {
    return I2C_WriteRegister8(DEV_IR, IR_REG_EFCR, IR_REG_EFCR_RXDISABLE | IR_REG_EFCR_TXDISABLE);
}

Result IrDev13::ResetTxRxFIFO() const {
    constinit static int resetFIFO = IR_REG_FCR_RESET_RX_FIFO | IR_REG_FCR_RESET_TX_FIFO;
    return I2C_WriteRegister8(DEV_IR, IR_REG_FCR, resetFIFO);
}

uint8_t IrDev13::ReadIER() const {
    uint8_t buf = 0;
    I2C_ReadRegister8(DEV_IR, IR_REG_IER, &buf);

    return buf;
}

Result IrDev13::WriteIER(const uint8_t IER) const {
    uint8_t finalVal = IER;
    if (mSleep) {
        finalVal |= IR_REG_IER_SLEEP;
    } else {
        finalVal &= ~IR_REG_IER_SLEEP;
    }

    return I2C_WriteRegister8(DEV_IR, IR_REG_IER, finalVal);
}

Result IrDev13::EnableTHRInterrupts() const {
    constinit static int fcr = IR_REG_FCR_RESET_RX_FIFO \
        | IR_REG_FCR_RESET_TX_FIFO \
        | IR_REG_FCR_FIFO_EN;

    I2C_WriteRegister8(DEV_IR, IR_REG_FCR, fcr);
    I2C_WriteRegister8(DEV_IR, IR_REG_EFCR, IR_REG_EFCR_RXDISABLE);
    return WriteIER(IR_REG_IER_THR_INT);
}

Result IrDev13::EnableRHRInterrupts() const {
    constinit static int fcr = IR_REG_FCR_RESET_RX_FIFO \
        | IR_REG_FCR_RESET_TX_FIFO \
        | IR_REG_FCR_FIFO_EN;

    I2C_WriteRegister8(DEV_IR, IR_REG_FCR, fcr);
    I2C_WriteRegister8(DEV_IR, IR_REG_EFCR, IR_REG_EFCR_TXDISABLE);
    return WriteIER(IR_REG_IER_RHR_INT | IR_REG_IER_RLS_INT);
}

Result IrDev13::PowerDownPin(const uint8_t powDown) {
    auto pinState = mOutputPinState;
    if (powDown) {
        pinState &= ~1;
    } else {
        pinState |= 1;
    }
    mOutputPinState = pinState;

    Result ret = I2C_WriteRegister8(DEV_IR, IR_REG_IOSTA, pinState);
    svcSleepThread(200000LL);
    return ret;
}

Result IrDev13::SetTriggerLvls(uint8_t txLvl, uint8_t rxLvl) const {
    if (!mInitialized) {
        return 0xD8210FF8;
    }

    /* As per manual, needs to be configured for N/4 */
    txLvl = txLvl / 4;
    rxLvl = rxLvl / 4;

    const uint8_t tlrVal = rxLvl << 4 | txLvl;

    return I2C_WriteRegister8(DEV_IR, IR_REG_TLR, tlrVal);
}

uint16_t IrDev13::GetDivisor() const {
    /* read original LCR Value */
    uint8_t origLcrValue = 0;
    I2C_ReadRegister8(DEV_IR, IR_REG_LCR, &origLcrValue);

    /* now latch */
    I2C_WriteRegister8(DEV_IR, IR_REG_LCR, origLcrValue | IR_REG_LCR_DIV_LAT_EN);

    uint8_t divisorLow = 0, divisorHigh = 0;

    I2C_ReadRegister8(DEV_IR, IR_REG_SPECIAL_DLL, &divisorLow);
    I2C_ReadRegister8(DEV_IR, IR_REG_SPECIAL_DLH, &divisorHigh);

    /* Restore original LCR value */
    I2C_WriteRegister8(DEV_IR, IR_REG_LCR, origLcrValue);

    return (divisorHigh << 8) | divisorLow;
}

Result IrDev13::SetDivisor(const uint16_t divisor) const {
    /* read original LCR Value */
    uint8_t origLcrValue = 0;
    I2C_ReadRegister8(DEV_IR, IR_REG_LCR, &origLcrValue);

    /* now latch */
    I2C_WriteRegister8(DEV_IR, IR_REG_LCR, origLcrValue | IR_REG_LCR_DIV_LAT_EN);

    const uint8_t divisorLow = divisor & 0xFF;
    const uint8_t divisorHigh = (divisor >> 8) & 0xFF;

    I2C_WriteRegister8(DEV_IR, IR_REG_SPECIAL_DLL, divisorLow);
    I2C_WriteRegister8(DEV_IR, IR_REG_SPECIAL_DLH, divisorHigh);

    /* Restore original LCR value */
    I2C_WriteRegister8(DEV_IR, IR_REG_LCR, origLcrValue);

    /* 
        Due to some reason, Nintendo calls GetDivisor here 
        do the same.
    */
    GetDivisor();

    return 0;
}

Result IrDev13::SetupCommunication() const {
    if (!mInitialized) {
        return 0xD8210FF8;
    }

    constinit static uint8_t lcrRegVal = 0xFF & ~IR_REG_LCR_BREAK_CTL;

    Result res = I2C_WriteRegister8(DEV_IR, IR_REG_LCR, lcrRegVal);
    if (R_FAILED(res))
        return res;

    /* enable enhanced functions */
    res = I2C_WriteRegister8(DEV_IR, IR_REG_EFR, IR_REG_EFR_ENHAN_EN);
    if (R_FAILED(res))
        return res;

    const uint8_t newLcrReg = mWordLen | IR_REG_LCR_EN_PARITY(mEnableParity) \
                                | IR_REG_LCR_EN_STOP(mStopBit);

    res = I2C_WriteRegister8(DEV_IR, IR_REG_LCR, newLcrReg);
    if (R_FAILED(res))
        return res;

    /* set mode */
    const uint8_t mcrRegVal = IR_REG_MCR_TCRTLR_EN | IR_REG_MCR_SET_MODE(mMode);
    res = I2C_WriteRegister8(DEV_IR, IR_REG_MCR, mcrRegVal);
    if (R_FAILED(res))
        return res;

    SetDivisor(mDivisor);

    /* Reset EFCR */
    res = I2C_WriteRegister8(DEV_IR, IR_REG_EFCR, 0);
    if (R_FAILED(res))
        return res;

    res = ResetTxRxFIFO();
    if (R_FAILED(res))
        return res;

    /* Enable FIFO */
    res = I2C_WriteRegister8(DEV_IR, IR_REG_FCR, IR_REG_FCR_FIFO_EN);
    if (R_FAILED(res))
        return res;

    SetTriggerLvls(mTxLvl, mRxLvl);

    /* Disable FIFO */
    res = I2C_WriteRegister8(DEV_IR, IR_REG_FCR, 0);
    if (R_FAILED(res))
        return res;

    res = ResetTxRxFIFO();
    if (R_FAILED(res))
        return res;

    WriteIER(0);

    return 0;
}

Result IrDev13::GoToSleep() {
    DisableTxRx();

    Result res = PowerDownPin(1);
    if (R_FAILED(res))
        return res;

    res = ResetTxRxFIFO();
    if (R_FAILED(res))
        return res;

    if (mInitialized) {
        mSleep = 1;
        WriteIER(ReadIER());
    } else {
        return 0xD8210FF8;
    }

    return 0;
}

Result IrDev13::Wakeup() {
    if (!mInitialized) {
        return 0xD8210FF8;
    }

    mSleep = 0;
    WriteIER(ReadIER());

    return PowerDownPin(0);
}

Result IrDev13::SetSleepMode(const uint8_t sleep) {
    if (!mInitialized) {
        return 0xD8210FF8;
    }

    mSleep = sleep;
    WriteIER(ReadIER());

    return 0;
}

Result IrDev13::SetIrLedState(const uint8_t enable) const {
    const uint32_t val = enable ? 1024 : 0;
    return GPIOIR_SetData(val, 1024);
}

uint8_t IrDev13::GetIrLedRecvState() const {
    uint32_t val = 0;
    GPIOIR_GetData(2048, &val);
    return val == 0;
}

void IrDev13::InitializeGpio() const {
    /* I have no clue what any of this does */
    GPIOIR_SetRegPart1(1024, 1024);
    SetIrLedState(0);
    GPIOIR_SetRegPart1(0, 2048);
    GPIOIR_SetRegPart1(0, 128);
    GPIOIR_SetRegPart2(0, 128);
    GPIOIR_SetInterruptMask(128, 128);
}

} // End of namespace driver
} // End of namespace ir
