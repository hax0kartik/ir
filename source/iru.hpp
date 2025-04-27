#pragma once

#include <3ds.h>

namespace ir {
namespace u {

enum TransferState : uint8_t {
    Ready = 1,
    DataSendInProg = 2,
    DataRecvInProg = 3,
};

enum Events {
    SendEvent = 0,
    RecieveEvent = 1,
};

struct irU {
    uint8_t initialized {};
    uint8_t autoPowerCtl {};
    uint8_t errorStatus {};
    uint8_t terminateThread {};
    uint8_t cancel {};
    TransferState transferState {};
    Handle events[2] {};
    Handle wakeUpThreadEvent {};
    Handle threadTerminatedEvent {};

    irU () {
        initialized = 0;
        autoPowerCtl = 0;
        transferState = TransferState::Ready;
        errorStatus = 0;
        events[0] = 0;
        events[1] = 0;
        wakeUpThreadEvent = 0;
        threadTerminatedEvent = 0;
    }
};

void ThreadFunc();

extern irU gIrU;

namespace commands {

Result Initialize();
Result ShutDown();
Result StartSendTransfer();
Result WaitSendTransfer();
Result StartRecvTransfer();
Result WaitRecvTransfer();
Result GetRecvTransferCount();
Result SetBaudRate(uint8_t baudRate);
Result GetBaudRate();
Result SetIRLedState(uint8_t enable);
Result GetIRLedRecvState(uint8_t *state);
Handle GetSendFinishedEvent();
Handle GetRecvFinishedEvent();
TransferState GetTransferState();
uint8_t GetErrorStatus();
Result SetAutomaticPowerControl(uint8_t ctl);
Result SetSleepModeState(uint8_t wakeup);

} // End of namespace commands

} // End of namespace u
} // End of namespace ir