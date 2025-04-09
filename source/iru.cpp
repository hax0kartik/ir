#include "iru.hpp"

namespace ir::u {
irU g_irU;
}

namespace ir::u::commands {

const Result Initialize() {
    return 0;
}

const uint8_t GetErrorStatus() {
    return g_irU.mErrorStatus;
}

} // End of namespace ir
