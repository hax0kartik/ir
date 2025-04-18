#pragma once

#include "../ir.hpp"
#include "../irrst.hpp"

namespace ir::rst {

void Handle() {
    auto *cmdbuf = getThreadCommandBuffer();
    const auto cmdid = cmdbuf[0] >> 16;

    switch (cmdid) {
        default:
            cmdbuf[1] = 0xD900182F;
            break;
    }
}

}
