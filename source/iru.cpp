#include "iru.hpp"
#include "ir.hpp"

namespace ir::u {
irU gIrU;

void ThreadFunc() {
    auto GoBackToSleep = [&]() -> Result {
        Result ret = gIr.GetIrDev13Driver().ResetTxRxFIFO();
        if (R_FAILED(ret)) {
            ret = 0xF9610C02;
            gIr.IrDriverError() = 1;
        }

        if (R_SUCCEEDED(ret) && gIrU.autoPowerCtl) {
            ret = gIr.GetIrDev13Driver().GoToSleep();
            if (R_FAILED(ret)) {
                ret = 0xF9610C02;
                gIr.IrDriverError() = 1;
            }
        }

        return ret;
    };

    while (1) {
        int8_t event = -1;
        Result ret = 0;

        svcWaitSynchronization(gIrU.wakeUpThreadEvent, -1LL);

        /* This is requested by ShutDown */
        if (gIrU.terminateThread) {
            /* Signal that we have exited */
            svcSignalEvent(gIrU.threadTerminatedEvent);
            return;
        }

        /* Wakeup only if autoPowerCtl is true */
        if (gIrU.autoPowerCtl) {
            ret = gIr.GetIrDev13Driver().Wakeup();
            if (R_FAILED(ret)) {
                gIr.IrDriverError() = 1;
                continue;
            }
        }

         /* Enable Interrupts, Send/Recieve*/
        if (gIrU.transferState == TransferState::DataSendInProg) {
            /* official ir - Wakeup is performed here */

            ret = gIr.GetIrDev13Driver().EnableTHRInterrupts();
            if (R_FAILED(ret)) {
                gIr.IrDriverError() = 1;
                gIrU.errorStatus = ErrorStatus::SendError;
            } else {
                DoSend();
                event = Events::SendEvent;
            }

        } else if (gIrU.transferState == TransferState::DataRecvInProg) {
            /* official ir - Wakeup is performed here */

            ret = gIr.GetIrDev13Driver().EnableRHRInterrupts();
            if (R_FAILED(ret)) {
                gIr.IrDriverError() = 1;
                gIrU.errorStatus = ErrorStatus::RecieveError;
            } else {
                DoRecieve();
                event = Events::RecieveEvent;
            }
        }

        GoBackToSleep();

        if (!gIrU.cancel && event >= 0) {
            svcSignalEvent(gIrU.events[event]);
        }

        gIrU.transferState = TransferState::Ready;
    }
}

void DoSend() {

}

void DoReceieve() {

}

} // End of namespace ir::u

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
    gIrU.errorStatus = ErrorStatus::NoError;
    gIrU.autoPowerCtl = 1;

    for (int i = 0; i < 2; i++) {
        res = svcCreateEvent(&gIrU.events[i], RESET_ONESHOT);
    }

    res = svcCreateEvent(&gIrU.wakeUpThreadEvent, RESET_ONESHOT);
    res = svcCreateEvent(&gIrU.threadTerminatedEvent, RESET_ONESHOT);

    res = SetBaudRate(BaudRate::B48000);
    if (R_FAILED(res))
        return res;

    gIrU.cancel = 0;
    gIrU.terminateThread = 0;
    gIrU.initialized = 1;

    return res;
}

Result ShutDown() {
    return 0;
}

Result GetBaudRate(uint8_t *baudrate) {
    if (gIr.HasIrDriverErrored()) {
        return 0xF9610C02;
    }

    if (gIr.InvalidState()) {
        return 0xC8A10C01;
    }

    if (gIrU.transferState == TransferState::Ready) {
        return gIr.GetBaudRate(reinterpret_cast<BaudRate *>(baudrate));
    }

    return 0xC8A10FF0;
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

Result SetAutomaticPowerControl(uint8_t ctl) {
    if (gIrU.transferState != TransferState::Ready) {
        return 0xC8A10FF0;
    }

    gIrU.autoPowerCtl = ctl;

    return 0;
}

Result SetIRLedState(uint8_t enable) {
    if (gIr.HasIrDriverErrored()) {
        return 0xF9610C02;
    }

    if (gIr.InvalidState()) {
        return 0xC8A10C01;
    }

    if (gIrU.transferState != TransferState::Ready) {
        return 0xC8A10FF0;
    } else {
        return gIr.GetIrDev13Driver().SetIrLedState(enable);
    }

    return 0;
}

Result GetIRLedRecvState(uint8_t *state) {
    if (gIr.HasIrDriverErrored()) {
        return 0xF9610C02;
    }

    if (gIr.InvalidState()) {
        return 0xC8A10C01;
    }

    if (gIrU.transferState != TransferState::Ready) {
        return 0xC8A10FF0;
    } else {
        *state = gIr.GetIrDev13Driver().GetIrLedRecvState();
        return 0;
    }

    return 0;
}

Result SetSleepModeState(const uint8_t wakeup) {
    if (gIr.HasIrDriverErrored()) {
        return 0xF9610C02;
    }

    if (gIr.InvalidState()) {
        return 0xC8A10C01;
    }

    Result ret = 0;

    if (gIrU.transferState != TransferState::Ready) {
        return 0xC8A10FF0;
    } else {
        if (wakeup) {
            ret = gIr.GetIrDev13Driver().Wakeup();
        } else {
            ret = gIr.GetIrDev13Driver().GoToSleep();
        }

        if (R_FAILED(ret)) {
            gIr.IrDriverError() = 1;
            ret = 0xF9610C02;
        }
    }

    return ret;
}

} // End of namespace ir
