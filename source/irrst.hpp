#pragma once

namespace ir {

struct irRst {
};

namespace commands {

Result GetHandles(Handle *sharedMemHandle, Handle *eventHandle);
Result Initialize(int updatePeriod, uint8_t calibrate);
Result ShutDown();

} // End of namespace commands

} // End of namespace ir