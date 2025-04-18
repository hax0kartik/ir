#pragma once
#include <3ds.h>
#include "irdriver.hpp"

namespace ir {
using namespace driver;

enum Services {
    IRU = 0,
    IRUSER,
    IRRST,
    MAX
};

struct irState {
    uint8_t readBufErr {};
    IrDev13 irDev13Driver {};

    irState () : irDev13Driver() {
        readBufErr = 0;
    }
};

extern struct irState gIr;

} // End of namespace ir
