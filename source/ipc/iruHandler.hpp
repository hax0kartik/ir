#pragma once

#include "../ir.hpp"
#include "../iru.hpp"

namespace ir::u {

void Handle() {
    auto *cmdbuf = getThreadCommandBuffer();
    const auto cmdid = cmdbuf[0] >> 16;

    switch (cmdid) {
        case 0x1: 
            ir::u::commands::Initialize();
            break;
        
        default:
            cmdbuf[1] = 0xD900182F;
            break;
    }
}

} // End of namespace ir
