#pragma once
#include <3ds.h>

#define DEV_IR 13

#define IR_REG_IER   0x8
#define IR_REG_FCR   0x10
#define IR_REG_IIR   0x10
#define IR_REG_EFR   0x10
#define IR_REG_LCR   0x18
#define IR_REG_MCR   0x20
#define IR_REG_LSR   0x28
#define IR_REG_SPR   0x38
#define IR_REG_TLR   0x38
#define IR_REG_TXLVL 0x40
#define IR_REG_RXLVL 0x48
#define IR_REG_IODIR 0x50
#define IR_REG_IOSTA 0x58
#define IR_REG_IOCTL 0x70
#define IR_REG_EFCR  0x78

#define IR_REG_SPECIAL_DLL 0x0
#define IR_REG_SPECIAL_DLH 0x8

/* Bits for IER Register */
#define IR_REG_IER_RHR_INT BIT(0)
#define IR_REG_IER_THR_INT BIT(1)
#define IR_REG_IER_RLS_INT BIT(2)
#define IR_REG_IER_MSR_INT BIT(3)
#define IR_REG_IER_SLEEP   BIT(4)

/* Bits for MCR Register */
#define IR_REG_MCR_DTR         BIT(0)
#define IR_REG_MCR_RTS         BIT(1)
#define IR_REG_MCR_TCRTLR_EN   BIT(2)
/* Bit 3 -> reserved */
#define IR_REG_MCR_LOOPBACK_EN BIT(4)
#define IR_REG_MCR_XON_ANY     BIT(5)
#define IR_REG_MCR_MODE        BIT(6)
#define IR_REG_CLOCK_DIV       BIT(7)

#define IR_REG_MCR_SET_MODE(x) (x << 6)

/* Bits for LCR Register */
#define IR_REG_LCR_NSTOP      BIT(2)
#define IR_REG_LCR_PAR_EN     BIT(3)
#define IR_REG_LCR_PAR_SEL    BIT(4)
#define IR_REG_LCR_FORCED_PAR BIT(5)
#define IR_REG_LCR_BREAK_CTL  BIT(6)
#define IR_REG_LCR_DIV_LAT_EN BIT(7)

#define IR_REG_LCR_EN_STOP(x)    (x << 2)
#define IR_REG_LCR_EN_PARITY(x)  (x << 3)
#define IR_REG_LCR_SET_PARITY(x) (x << 4)

/* Bits for EFR Register */
#define IR_REG_EFR_ENHAN_EN BIT(4)
#define IR_REG_EFR_SPEC_DET BIT(5)
#define IR_REG_EFR_RTS_EN   BIT(6)
#define IR_REG_EFR_CTS_EN   BIT(7)

/* Bits for FCR Register */
#define IR_REG_FCR_FIFO_EN       BIT(0)
#define IR_REG_FCR_RESET_RX_FIFO BIT(1)
#define IR_REG_FCR_RESET_TX_FIFO BIT(2)

/* Bits for IOControl Register */
#define IR_REG_IOCTL_IOLATCH_EN  BIT(0)

/* Bits for EFCR Regiser */
#define IR_REG_EFCR_9BIT_EN   BIT(0)
#define IR_REG_EFCR_RXDISABLE BIT(1)
#define IR_REG_EFCR_TXDISABLE BIT(2)

/* Values for the Parity Select bit of LCR Register */
#define IR_REG_LCR_PARITY_ODD 0
#define IR_REG_LCR_PARITY_EVEN 1

/* Values for the wordlen[0:1] bits of the LCR Register */
#define IR_REG_LCR_WORD_LEN5 0
#define IR_REG_LCR_WORD_LEN6 1
#define IR_REG_LCR_WORD_LEN7 2
#define IR_REG_LCR_WORD_LEN8 3

/* Values for the stop bit of the LCR Register */
#define IR_REG_LCR_STOP1 0
#define IR_REG_LCR_STOP2 1

/* Values for the clock divisor bit of MCR Register */
#define IR_REG_MCR_CLK_DIV1 0
#define IR_REG_MCR_CLK_DIV4 1

/* Values for the mode bit of the MCR Register */
#define IR_REG_MCR_MODE_UART 0
#define IR_REG_MCR_MODE_IRDA 1
