#include <new>
#include "irrst.hpp"
#include "ir.hpp"

namespace ir::rst {
irRst gIrRst;

Result Initialize() {
    gIrRst.latestKeys = 0;
    gIrRst.ring = 0;

    svcCreateEvent(&gIrRst.eventHandle, RESET_ONESHOT);

    /* Create shared memory block */
    Result ret = svcCreateMemoryBlock(&gIrRst.sharedMemHandle, 0, \
        0x1000, (MemPerm)(MEMPERM_READ | MEMPERM_WRITE), MEMPERM_READ);

    if (R_SUCCEEDED(ret)) {
        mappableInit(0x10000000, 0x14000000);
        gIrRst.sharedMemAddr = static_cast<uint32_t *>(mappableAlloc(0x1000));

        if (gIrRst.sharedMemAddr) {
            ret = svcMapMemoryBlock(gIrRst.sharedMemHandle, \
                reinterpret_cast<uint32_t>(gIrRst.sharedMemAddr), \
                (MemPerm)(MEMPERM_READ | MEMPERM_WRITE), MEMPERM_DONTCARE);

            if (R_FAILED(ret)) {
                return ret;
            }
        } else {
            return -2;
        }

    } else {
        return -1;
    }

    gIrRst.ring = new (gIrRst.sharedMemAddr)Ring;

    return 0;
}

namespace commands {

Result GetHandles(Handle *sMemHandle, Handle *evtHandle) {
    /* This can never really happen but for accuracy's sake, do what official ir does */
    if (!*sMemHandle || !*evtHandle) {
        return 0xC8610FF3;
    }

    *sMemHandle = gIrRst.sharedMemHandle;
    *evtHandle = gIrRst.eventHandle;

    return 0;
}

} // End of namespace commands
} // End of namespace ir::rst