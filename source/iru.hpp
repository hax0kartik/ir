#pragma once

#include <3ds.h>

namespace ir {
namespace u {

enum TransferState {
    Ready = 1,
    DataSendInProg = 2,
    DataRecvInProg = 3,
};

struct irU {
    TransferState mTransferState;
    uint8_t mErrorStatus;

    irU () {
        mTransferState = Ready;
        mErrorStatus = 0;
    }
};

extern irU g_irU;

namespace commands {

const Result Initialize();
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
const uint8_t GetErrorState();
Result SetSleepModeActive();
Result SetSleepModeState();

} // End of namespace commands

} // End of namespace u
} // End of namespace ir