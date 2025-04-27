#pragma once
#include <3ds.h>

#define DEV_CPP 17

#define IR_CMD_UNK_x05         0x05
#define IR_CMD_UNK_x08         0x08
#define IR_CMD_UNK_x33         0x33
#define IR_CMD_UNK_x37         0x37

#define IR_CMD_UNK_x52         0x52
#define IR_CMD_UNK_xC1         0xC1

#define IR_CMD_SCAN_PERIOD     0xA0

/* 0xD0 and 0xD2 are related */
#define IR_CMD_UNK_xD0         0xD0
#define IR_CMD_UNK_xD2         0xD2
#define IR_CMD_UNK_x7C         0x7C
#define IR_CMD_UNK_x7F         0x7F
/* 0xF0, 0xF1 and 0xF2 are related */
#define IR_CMD_UNK_xF0         0xF0
#define IR_CMD_UNK_xF1         0xF1
#define IR_CMD_UNK_xF2         0xF2
#define IR_CMD_UNK_xF3         0xF3
/* 0xF5 seems to be a R/W command.*/
#define IR_CMD_UNK_xF5         0xF5
#define IR_CMD_ENABLE_POLLING  0xF7
#define IR_CMD_DISABLE_POLLING 0xF9
/* 0xFA is related to polling or getting controls state */
#define IR_CMD_UNK_xFA         0xFA
