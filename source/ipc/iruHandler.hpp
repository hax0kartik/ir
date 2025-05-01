#pragma once

#include "../ir.hpp"
#include "../iru.hpp"

namespace ir::u {

void Handle() {
    auto *cmdbuf = getThreadCommandBuffer();
    const auto cmdid = cmdbuf[0] >> 16;

    switch (cmdid) {
        case 0x1: 
            cmdbuf[0] = 0x10040;
            cmdbuf[1] = commands::Initialize();
            break;

        case 0x2:
            cmdbuf[0] = 0x20040;
            cmdbuf[1] = commands::ShutDown();
            break;

        case 0x9:
            cmdbuf[0] = 0x90040;
            cmdbuf[1] = commands::SetBaudRate(cmdbuf[1] & 0xFF);
            break;

        case 0xA: {
            uint8_t baudRate;
            cmdbuf[0] = 0xA0080;
            cmdbuf[1] = commands::GetBaudRate(&baudRate);
            cmdbuf[2] = baudRate;
            break;
        }

        case 0xB:
            cmdbuf[0] = 0xB0040;
            cmdbuf[1] = commands::SetIRLedState(static_cast<uint8_t>(cmdbuf[1]));
            break;

        case 0xC: {
            uint8_t state;
            cmdbuf[0] = 0xC0080;
            cmdbuf[1] = commands::GetIRLedRecvState(&state);
            cmdbuf[2] = state;
            break;
        }

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

        case 0x10:
            cmdbuf[0] = 0x100080;
            cmdbuf[1] = 0;
            cmdbuf[2] = commands::GetErrorStatus();
            break;

        case 0x11:
            cmdbuf[0] = 0x110040;
            cmdbuf[1] = commands::SetAutomaticPowerControl(cmdbuf[1] & 0xFF);
            break;

        case 0x12:
            cmdbuf[0] = 0x120040;
            cmdbuf[1] = commands::SetSleepModeState(cmdbuf[1]);
            break;

        default:
            cmdbuf[1] = 0xD900182F;
            break;
    }
}

} // End of namespace ir
