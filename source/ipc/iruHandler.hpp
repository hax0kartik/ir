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

        case 0x9:
            cmdbuf[0] = 0x90000;
            cmdbuf[1] = commands::SetBaudRate(cmdbuf[1] & 0xFF);
            break;

        case 0xD:
            cmdbuf[0] = 0xD0042;
            cmdbuf[1] = 0;
            cmdbuf[2] = 0;
            cmdbuf[3] = commands::GetSendFinishedEvent();
            break;

        case 0xE:
            cmdbuf[0] = 0xE0042;
            cmdbuf[1] = 0;
            cmdbuf[2] = 0;
            cmdbuf[3] = commands::GetRecvFinishedEvent();
            break;

        case 0xF:
            cmdbuf[0] = 0xF0080;
            cmdbuf[1] = 0;
            cmdbuf[2] = commands::GetTransferState();
            break;

        default:
            cmdbuf[1] = 0xD900182F;
            break;
    }
}

} // End of namespace ir
