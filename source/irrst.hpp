#pragma once
#include <3ds.h>
#include "PadRing.hpp"

namespace ir {
namespace rst {

struct irRst {
    uint32_t latestKeys;
    Ring *ring;
    uint32_t *sharedMemAddr;
    Handle sharedMemHandle;
    Handle eventHandle;
};

Result Initialize();

extern irRst gIrRst;

namespace commands {

Result GetHandles(Handle *sMemHandle, Handle *evtHandle);
Result Initialize(int updatePeriod, uint8_t calibrate);
Result ShutDown();

} // End of namespace commands

} // End of namespace rst
} // End of namespace ir