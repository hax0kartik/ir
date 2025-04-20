#pragma once
#include <3ds.h>
#include "dev13.hpp"

namespace ir {
namespace driver {

#define IR_DEFAULT_TRIGGER_LVL = 40;

class IrDev13 {
    uint8_t mInitialized {};
    uint8_t mSleep {};
    uint16_t mDivisor {};
    uint8_t mWordLen {};
    uint8_t mStopBit {};
    uint8_t mEnableParity {};
    uint8_t mMode {};
    uint8_t mOutputPinState {};
    uint8_t mTxLvl {};
    uint8_t mRxLvl {};

    public:
    IrDev13() {
        mInitialized = 0;
        mSleep = 0;
        mWordLen = IR_REG_LCR_WORD_LEN8;
        mStopBit = IR_REG_LCR_STOP1;
        mEnableParity = 0;
        mMode = IR_REG_MCR_MODE_IRDA;
        mTxLvl = IR_DEFAULT_TRIGGER_LVL;
        mRxLvl = IR_DEFAULT_TRIGGER_LVL;
    }

    /* 
    We only want a single instance of this class.
    Probably we should make this a singleton?
    */
    IrDev13(IrDev13 &) = delete;
    void operator=(const IrDev13 &) = delete;

    Result Initialize();
    void ResetEFCR() const;
    uint8_t ReadIER() const;
    void WriteIER(uint8_t IER) const;
    void SetDivisor(uint16_t divisor) const;
    uint16_t GetDivisor() const;
    Result SetTriggerLvls(uint8_t txLvl, uint8_t rxLvl) const;
    Result IOLatchEnable() const;
    Result ResetEFCR() const;
    Result SetOutputPinsState(uint8_t state) const;
    Result ResetTxRxFIFO() const;

    Result SetupCommunication() const;
    Result GoToSleep();
};

} // End of namespace driver
} // End of namespace ir
