#include "iru.hpp"
#include "ir.hpp"

namespace ir::u {
irU gIrU;
}

namespace ir::u::commands {

Result Initialize() {
    Result res = 0;

    if (gIr.readBufErr) {
        return 0xF9610C02;
    }

    if (gIrU.initialized == 1) {
        return 0xD8210FF9;
    }

    /* Mark as ready to transfer and automaticcaly control power */
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

uint8_t GetErrorStatus() {
    return gIrU.errorStatus;
}

} // End of namespace ir
