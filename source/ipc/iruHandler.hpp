#pragma once

#include "../ir.hpp"
#include "../iru.hpp"

namespace ir::u {

void Handle() {
    auto *cmdbuf = getThreadCommandBuffer();
    const auto cmdid = cmdbuf[0] >> 16;

    switch (cmdid) {
        case 0x1: 
            cmdbuf[1] = commands::Initialize();
            break;

        case 0x2:
            cmdbuf[1] = commands::ShutDown();
            break;


        default:
            cmdbuf[1] = 0xD900182F;
            break;
    }
}

} // End of namespace ir
