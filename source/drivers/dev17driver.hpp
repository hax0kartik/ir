#pragma once
#include <3ds.h>
#include "dev17.hpp"

namespace ir {
namespace driver {

enum State : uint8_t {
    ReadyToInit = 0,
    Sleep = 1,
    Awake = 2,
};

class IrDev17 {
    uint8_t mState;
    uint8_t mPoll;
    uint8_t mWasCmdxD0;
    uint8_t mVariant;

    public:

    struct Options {
        uint8_t unk1;
        uint8_t unk2;
        uint8_t unk3;
        uint8_t unk4;
        uint8_t unk5;
        uint8_t unk6;
    };

    Result Initialize();
    Result Sleep(int a2);
    Result Wakeup(int enablePolling);
    Result SetOptions(const Options *options);
};

} // End of namespace driver
} // End of namespace ir
