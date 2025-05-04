#include "ir.hpp"
#include "irrst.hpp"

namespace ir {
irC gIr;

void Initialize() {
    gIr.GetIrDev13Driver().Initialize();
    gIr.GetCppMaster().Initialize();
    rst::Initialize();
}

Result irC::SetBaudRate(BaudRate baudRate) {

    uint16_t divisor = 0;

    switch (baudRate) {
        case BaudRate::B115200:
            divisor = IR_DIVISOR_115200;
            break;

        case BaudRate::B96000:
            divisor = IR_DIVISOR_96000;
            break;

        case BaudRate::B72000:
            divisor = IR_DIVISOR_72000;
            break;

        case BaudRate::B48000:
            divisor = IR_DIVISOR_48000;
            break;

        case BaudRate::B36000:
            divisor = IR_DIVISOR_36000;
            break;

        case BaudRate::B24000:
            divisor = IR_DIVISOR_24000;
            break;

        case BaudRate::B18000:
            divisor = IR_DIVISOR_18000;
            break;

        case BaudRate::B12000:
            divisor = IR_DIVISOR_12000;
            break;

        case BaudRate::B9600:
            divisor = IR_DIVISOR_9600;
            break;

        case BaudRate::B6000:
            divisor = IR_DIVISOR_6000;
            break;

        case BaudRate::B3000:
            divisor = IR_DIVISOR_3000;
            break;

        case BaudRate::B57600:
            divisor = IR_DIVISOR_57600;
            break;

        case BaudRate::B38400:
            divisor = IR_DIVISOR_38400;
            break;

        case BaudRate::B19200:
            divisor = IR_DIVISOR_19200;
            break;

        case BaudRate::B7200:
            divisor = IR_DIVISOR_7200;
            break;

        case BaudRate::B4800:
            divisor = IR_DIVISOR_4800;
            break;

        default:
            return 0xE0E10FED;
    }

    const Result res = mIrDev13Driver.SetDivisor(divisor);
    if (R_FAILED(res)) {
        mIrDriverErr = 1;
        return 0xF9610C02;
    }

    return 0;
}

Result irC::GetBaudRate(BaudRate *baudrate) {
    uint16_t divisor = mIrDev13Driver.GetDivisor();
    switch (divisor) {
        case IR_DIVISOR_115200:
            *baudrate = BaudRate::B115200;
            break;

        case IR_DIVISOR_96000:
            *baudrate = BaudRate::B96000;
            break;
        
        case IR_DIVISOR_72000:
            *baudrate = BaudRate::B72000;
            break;

        case IR_DIVISOR_48000:
            *baudrate = BaudRate::B48000;
            break;

        case IR_DIVISOR_36000:
            *baudrate = BaudRate::B36000;
            break;

        case IR_DIVISOR_24000:
            *baudrate = BaudRate::B24000;
            break;

        case IR_DIVISOR_12000:
            *baudrate = BaudRate::B12000;
            break;

        case IR_DIVISOR_9600:
            *baudrate = BaudRate::B9600;
            break;

        case IR_DIVISOR_6000:
            *baudrate = BaudRate::B6000;
            break;

        case IR_DIVISOR_3000:
            *baudrate = BaudRate::B3000;
            break;

        case IR_DIVISOR_57600:
            *baudrate = BaudRate::B57600;
            break;

        case IR_DIVISOR_38400:
            *baudrate = BaudRate::B38400;
            break;

        case IR_DIVISOR_19200:
            *baudrate = BaudRate::B19200;
            break;

        case IR_DIVISOR_7200:
            *baudrate = BaudRate::B7200;
            break;

        case IR_DIVISOR_4800:
            *baudrate = BaudRate::B4800;
            break;
        
        default:
            *baudrate = BaudRate::INVALID;
            return 0xE0E10FED;
    }

    return 0;
}

} // End of namespace ir


