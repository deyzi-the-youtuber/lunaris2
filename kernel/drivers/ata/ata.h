#ifndef __ATA_H
#define __ATA_H

#include <stdint.h>

#define ATA_BUS1_PRIMARY_IO_PORT    0x1f0
#define ATA_BUS1_SECOND_IO_PORT     0x170

#define ATA_BUS2_PRIMARY_IO_PORT    0x1e8
#define ATA_BUS2_SECOND_IO_PORT     0x168

#define ATA_BUS1_PRIMARY_CTRL_REG   0x3f6
#define ATA_BUS1_SECONDARY_CTRL_REG 0x376

#define ATA_BUS2_PRIMARY_CTRL_REG   0x3e6
#define ATA_BUS2_SECOND_CTRL_REG    0x366


/*
* Status Register Layout
* Bits:
* 0    1    2     3    4    5   6    7
* [ERR][IDX][CORR][DRQ][SRV][DF][RDY][BSY] <- not sure if the eigth bit is zero or not
* 
*/

#define ATA_STATUS_ERR  (1 << 0)
#define ATA_STATUS_DRQ  (1 << 3)
#define ATA_STATUS_SRV  (1 << 4)
#define ATA_STATUS_DF   (1 << 5)
#define ATA_STATUS_RDY  (1 << 6)
#define ATA_STATUS_BUSY (1 << 7)

// ATA commands

#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xc8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xca
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xe7
#define ATA_CMD_CACHE_FLUSH_EXT   0xea
#define ATA_CMD_PACKET            0xa0
#define ATA_CMD_IDENTIFY_PACKET   0xa1
#define ATA_CMD_IDENTIFY          0xec

// ATAPI commands

#define ATAPI_CMD_READ            0xa8
#define ATAPI_CMD_EJECT           0x1b

// Identification space

#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200

#define ATA_PRIM_LEGACY (1 << 0)
#define ATA_SEC_LEGACY  (1 << 2)

// Command set stuff
#define ATA_48BIT_ADDR (1 << 26)

// Drive select

#define ATA_MASTER 0xa0
#define ATA_SLAVE 0xb0

struct ide_channel
{
   uint16_t io_base;      // I/O Base.
   uint16_t ctrl_base;    // Control Base
   uint16_t bmide;        // Bus Master IDE
   uint8_t no_int;        // nIEN (No Interrupt);
};

struct ide_drive
{
  uint8_t used;
  uint8_t channel;
  uint8_t drive;
  uint16_t type;
  uint16_t signature;
  uint16_t features;
  uint32_t commands;
  uint32_t size;
  uint8_t name[41];
};

#endif
