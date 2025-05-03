#pragma once

#include "../ir.hpp"
#include "../irrst.hpp"

namespace ir::rst {

void Handle() {
    auto *cmdbuf = getThreadCommandBuffer();
    const auto cmdid = cmdbuf[0] >> 16;

    switch (cmdid) {
        case 0x1:
            cmdbuf[0] = 0x10043;
            cmdbuf[1] = commands::GetHandles(&cmdbuf[3], &cmdbuf[4]);
            cmdbuf[2] = 0x4000000;
            break;

        default:
            cmdbuf[1] = 0xD900182F;
            break;
    }
}

}
