#include <3ds.h>

Result i2cIrInit();
void i2cIrFinalize();
Result I2C_EnableRegisterBits8(u8 devid, u8 regid, u8 enablemask);
Result I2C_DisableRegisterBits8(u8 devid, u8 regid, u8 disablemask);
Result I2C_WriteRegister8(u8 devid, u8 regid, u8 regdata);
Result I2C_ReadRegister8(u8 devid, u8 regid, u8 *regdata);
Result I2C_ReadRegisterBuffer8(u8 devid, u8 regid, u8 *buffer, size_t buffersize);
Result I2C_WriteRegisterBuffer(u8 devid, u8 regid, u8 *buffer, size_t buffersize);
Result I2C_ReadRegisterBuffer(u8 devid, u8 regid, u8 *buffer, size_t buffersize);