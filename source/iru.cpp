#include "iru.hpp"
#include "ir.hpp"

namespace ir::u {
irU gIrU;
}

namespace ir::u::commands {

Result Initialize() {
    Result res = 0;

    if (gIr.HasIrDriverErrored()) {
        return 0xF9610C02;
    }

    if (gIrU.initialized == 1) {
        return 0xD8210FF9;
    }

    /* Mark as ready to transfer and automatically control power */
    gIrU.transferState = TransferState::Ready;
    gIrU.errorStatus = 0;
    gIrU.autoPowerCtl = 1;

    for (int i = 0; i < 2; i++) {
        res = svcCreateEvent(&gIrU.events[i], RESET_ONESHOT);
    }

    return res;
}

Result ShutDown() {
    return 0;
}

Result SetBaudRate(uint8_t baudRate) {
    if (gIr.HasIrDriverErrored()) {
        return 0xF9610C02;
    }

    if (gIr.InvalidState()) {
        return 0xC8A10C01;
    }

    Result ret = 0;

    if (gIrU.transferState == TransferState::Ready) {
        /* 
            As per manual: Writing to the divisor latches, DLL and DLH, 
            to set the baud clock, must not be done during Sleep mode.
        */
        if (gIrU.autoPowerCtl) {
            ret = gIr.GetIrDev13Driver().SetSleepMode(0);
            if (R_FAILED(ret)) {
                ret = 0xF9610C02;
                gIr.IrDriverError() = 1;

                return ret;
            }
        }

        ret = gIr.SetBaudRate(static_cast<BaudRate>(baudRate));
        if (R_SUCCEEDED(ret)) {
            if (!gIrU.autoPowerCtl) {
                return 0;
            }

            ret = gIr.GetIrDev13Driver().SetSleepMode(1);
            if (R_FAILED(ret)) {
                /* 0xF9610C02 is not set here due to some reason */
                gIr.IrDriverError() = 1;

                return ret;
            }
        }

    } else {
        ret = 0xC8A10FF0;
    }

    return ret;
}

uint8_t GetErrorStatus() {
    return gIrU.errorStatus;
}

TransferState GetTransferState() {
    return gIrU.transferState;
}

Handle GetRecvFinishedEvent() {
    return gIrU.events[Events::RecieveEvent];
}

Handle GetSendFinishedEvent() {
    return gIrU.events[Events::SendEvent];
}

} // End of namespace ir
