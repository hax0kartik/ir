#pragma once
#include <3ds.h>
#include "drivers/dev17driver.hpp"
#include "exclusive_rw.hpp"

namespace ir {
using namespace driver;

enum CppMode : uint8_t {
    /* Not set */
    None = 0,

    /* All communications shall take place using ir:USER */
    IrUser = 1,

    /* Communication shall take place using ir:rst*/
    IrRst = 2
};

class CppMaster {
    uint8_t mMode;
    uint8_t mInitFailed;
    IrDev17 mIrDev17Driver;

    public:
    CppMaster() : mIrDev17Driver() {
        mMode = 0;
        mInitFailed = 0;
    }

    void Initialize() {
        if (R_FAILED(mIrDev17Driver.Initialize())) {
            mInitFailed = 1;
        }

        if (R_FAILED(mIrDev17Driver.Wakeup(0))) {
            mInitFailed = 1;
        }

        if (R_FAILED(mIrDev17Driver.Sleep(0))) {
            mInitFailed = 1;
        }

        ExclusiveWrite(&mMode, CppMode::None);
    }
};

} // End of namespace ir
