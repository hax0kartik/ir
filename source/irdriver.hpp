#pragma once
#include <3ds.h>
#include "dev13.hpp"

namespace ir {
namespace driver {

class IrDev13 {
    uint8_t mInitialized {};
    uint8_t mSleep {};
    uint16_t mDivisor {};
    uint8_t mWordLen {};
    uint8_t mStopBit {};
    uint8_t mEnableParity {};
    uint8_t mMode {};

    public:
    IrDev13() {
        mInitialized = 0;
        mSleep = 0;
        mWordLen = IR_REG_LCR_WORD_LEN8;
        mMode = IR_REG_MCR_MODE_IRDA;
        mStopBit = IR_REG_LCR_STOP1;
    }

    /* 
    We only want a single instance of this class.
    Probably we should make this a singleton?
    */
    IrDev13(IrDev13 &) = delete;
    void operator=(const IrDev13 &) = delete;

    Result Initialize();
    uint8_t ReadIER() const;
    void WriteIER(uint8_t IER) const;

    Result SetupCommunication() const;
};

} // End of namespace driver
} // End of namespace ir
