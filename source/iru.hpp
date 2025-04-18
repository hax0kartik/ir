#pragma once

#include <3ds.h>

namespace ir {
namespace u {

enum TransferState : uint8_t {
    Ready = 1,
    DataSendInProg = 2,
    DataRecvInProg = 3,
};

struct irU {
    uint8_t initialized {};
    uint8_t autoPowerCtl {};
    uint8_t errorStatus {};
    TransferState transferState {};
    Handle events[2] {};

    irU () {
        initialized = 0;
        autoPowerCtl = 0;
        transferState = TransferState::Ready;
        errorStatus = 0;
        events[0] = 0;
        events[1] = 0;
    }
};

extern irU gIrU;

namespace commands {

Result Initialize();
Result ShutDown();
Result StartSendTransfer();
Result WaitSendTransfer();
Result StartRecvTransfer();
Result WaitRecvTransfer();
Result GetRecvTransferCount();
Result SetBitRate();
Result GetBitRate();
Result SetIRLedState();
Result GetIRLedRecvState();
Result GetSendFinishedEvent();
Result GetRecvFinishedEvent();
Result GetTransferState();
uint8_t GetErrorState();
Result SetSleepModeActive();
Result SetSleepModeState();

} // End of namespace commands

} // End of namespace u
} // End of namespace ir