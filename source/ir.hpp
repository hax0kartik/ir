#pragma once
#include <3ds.h>
#include "cppMaster.hpp"
#include "drivers/dev13driver.hpp"

namespace ir {
using namespace driver;

enum Services {
    IRU = 0,
    IRUSER,
    IRRST,
    MAX
};

/* Bits per second */
enum BaudRate : uint8_t {
    B115200 = 3,
    B96000,
    B72000,
    B48000,
    B36000,
    B24000,
    B18000,
    B12000,
    B9600,
    B6000,
    B3000,

    B57600,
    B38400,
    B19200,
    B7200,
    B4800,
    INVALID
};

void Initialize();

class irC {
    uint8_t mIrDriverErr {};
    uint8_t mInvalidState {};
    IrDev13 mIrDev13Driver {};
    CppMaster mIrCppMaster {};

    public:
    irC () : mIrDev13Driver(), mIrCppMaster() {
        mInvalidState = 0;
        mIrDriverErr = 0;
    }

    IrDev13& GetIrDev13Driver() {
        return mIrDev13Driver;
    }

    CppMaster& GetCppMaster() {
        return mIrCppMaster;
    }

    uint8_t HasIrDriverErrored() const {
        return mIrDriverErr;
    }

    uint8_t& IrDriverError() {
        return mIrDriverErr;
    }

    uint8_t InvalidState() const {
        return mInvalidState;
    }

    Result SetBaudRate(BaudRate BaudRate);
    Result GetBaudRate(BaudRate *BaudRate);
};

extern class irC gIr;

} // End of namespace ir
