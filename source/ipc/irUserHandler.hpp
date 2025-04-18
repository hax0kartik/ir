#pragma once

#include "../ir.hpp"
#include "../iruser.hpp"

namespace ir::user {

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
