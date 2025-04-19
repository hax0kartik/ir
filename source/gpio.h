#include <3ds.h>

Result gpioIrInit(void);
void gpioIrExit();
Result GPIOIR_SetRegPart1(u32 value, u32 mask);
Result GPIOIR_SetRegPart2(u32 value, u32 mask);
Result GPIOIR_SetInterruptMask(u32 value, u32 mask);
Result GPIOIR_BindInterrupt(Handle *intr);
Result GPIOIR_UnbindInterrupt(Handle *intr);
Result GPIOIR_GetData(u32 mask, u32 *value);