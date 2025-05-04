#include "i2c.h"

Handle i2cIrHandle;
static int i2cIrRefCount;

Result i2cIrInit() {
    if (AtomicPostIncrement(&i2cIrRefCount))
        return 0;

    Result ret = 0;

    if (R_FAILED(ret = srvGetServiceHandle(&i2cIrHandle, "i2c::IR")))
        AtomicDecrement(&i2cIrRefCount);

    return ret;
}

void i2cIrSetHandle(Handle handle) {
    i2cIrHandle = handle;
}

void i2cIrFinalize() {
    AtomicDecrement(&i2cIrRefCount);
    svcCloseHandle(i2cIrHandle);
}

Result I2C_EnableRegisterBits8(u8 devid, u8 regid, u8 enablemask) {
    Result ret = 0;
    u32 *cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(2, 3, 0);
    cmdbuf[1] = devid;
    cmdbuf[2] = regid;
    cmdbuf[3] = enablemask;

    if (R_FAILED(ret = svcSendSyncRequest(i2cIrHandle)))
        return ret;

    return cmdbuf[1];
}

Result I2C_DisableRegisterBits8(u8 devid, u8 regid, u8 disablemask) {
    Result ret = 0;
    u32 *cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(3, 3, 0);
    cmdbuf[1] = devid;
    cmdbuf[2] = regid;
    cmdbuf[3] = disablemask;

    if (R_FAILED(ret = svcSendSyncRequest(i2cIrHandle)))
        return ret;

    return cmdbuf[1];
}

Result I2C_WriteRegister8(u8 devid, u8 regid, u8 regdata) {
    Result ret = 0;
    u32 *cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(5, 3, 0);
    cmdbuf[1] = devid;
    cmdbuf[2] = regid;
    cmdbuf[3] = regdata;

    if (R_FAILED(ret = svcSendSyncRequest(i2cIrHandle)))
        return ret;

    return cmdbuf[1];
}

Result I2C_WriteCommand8(u8 devid, u8 cmdid) {
    Result ret = 0;
    u32 *cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(6, 2, 0);
    cmdbuf[1] = devid;
    cmdbuf[2] = cmdid;

    if (R_FAILED(ret = svcSendSyncRequest(i2cIrHandle)))
        return ret;

    return cmdbuf[1];
}

Result I2C_ReadRegister8(u8 devid, u8 regid, u8 *regdata) {
    Result ret = 0;
    u32 *cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = IPC_MakeHeader(9, 2, 0);
    cmdbuf[1] = devid;
    cmdbuf[2] = regid;

    if (R_FAILED(ret = svcSendSyncRequest(i2cIrHandle)))
        return ret;

    *regdata = cmdbuf[2] & 0xFF;

    return cmdbuf[1];
}

Result I2C_ReadRegisterBuffer8(u8 devid, u8 regid, u8 *buffer, size_t buffersize) {
    Result ret = 0;
    u32 savedbufs[2];
    u32 *cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0xD00C0;
    cmdbuf[1] = devid;
    cmdbuf[2] = regid;
    cmdbuf[3] = buffersize;
    u32 *staticbuf = getThreadStaticBuffers();
    savedbufs[0] = staticbuf[0];
    savedbufs[1] = staticbuf[1];
    staticbuf[0] = (buffersize << 14) | 2;
    staticbuf[1] = (u32)buffer;

    if (R_FAILED(ret = svcSendSyncRequest(i2cIrHandle)))
        cmdbuf[1] = ret;

    staticbuf[0] = savedbufs[0];
    staticbuf[1] = savedbufs[1];
    return cmdbuf[1];
}

Result I2C_WriteRegisterBuffer(u8 devid, u8 regid, u8 *buffer, size_t buffersize) {
    Result ret = 0;
    u32 *cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0xE00C2;
    cmdbuf[1] = devid;
    cmdbuf[2] = regid;
    cmdbuf[3] = buffersize;
    cmdbuf[4] = (buffersize << 14) | 0x402;
    cmdbuf[5] = (u32)buffer;

    if (R_FAILED(ret = svcSendSyncRequest(i2cIrHandle)))
        return ret;

    return cmdbuf[1];
}

Result I2C_ReadRegisterBuffer(u8 devid, u8 regid, u8 *buffer, size_t buffersize) {
    Result ret = 0;
    u32 savedbufs[2];
    u32 *cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0xF00C0;
    cmdbuf[1] = devid;
    cmdbuf[2] = regid;
    cmdbuf[3] = buffersize;
    u32 *staticbuf = getThreadStaticBuffers();
    savedbufs[0] = staticbuf[0];
    savedbufs[1] = staticbuf[1];
    staticbuf[0] = (buffersize << 14) | 2;
    staticbuf[1] = (u32)buffer;

    if (R_FAILED(ret = svcSendSyncRequest(i2cIrHandle)))
        cmdbuf[1] = ret;

    staticbuf[0] = savedbufs[0];
    staticbuf[1] = savedbufs[1];
    return cmdbuf[1];
}

Result I2C_ReadRegisterRaw(u8 devid, u8 *buffer, size_t buffersize) {
    Result ret = 0;
    u32 savedbufs[2];
    u32 *cmdbuf = getThreadCommandBuffer();
    cmdbuf[0] = 0x150080;
    cmdbuf[1] = devid;
    cmdbuf[2] = buffersize;
    u32 *staticbuf = getThreadStaticBuffers();
    savedbufs[0] = staticbuf[0];
    savedbufs[1] = staticbuf[1];
    staticbuf[0] = (buffersize << 14) | 2;
    staticbuf[1] = (u32)buffer;

    if (R_FAILED(ret = svcSendSyncRequest(i2cIrHandle)))
        cmdbuf[1] = ret;

    staticbuf[0] = savedbufs[0];
    staticbuf[1] = savedbufs[1];
    return cmdbuf[1];
}