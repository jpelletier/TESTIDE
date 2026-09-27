#include <stdint.h>

#define PPIDE_DIR_READ	0x92
#define PPIDE_DIR_WRITE	0x80
#define PPIDE_CTL_RESET	0x80

//CTL_IDLE	00h
#define CTL_BYTE	0x08
#define CTL_FEAT	0x09
#define CTL_DRHD	0x0A
#define CTL_LBA_LOW	0x0B
#define CTL_LBA_MID	0x0C
#define CTL_LBA_HIG	0x0D
#define CTL_MAS_LBA	0x0E
#define CTL_CMD		0x0F

#define CTL_PRELOAD 0x08
#define CTL_ERROR   0x09
#define CTL_STATUS	0x0F

#define PPIDE_CMD_NOP		0x00
#define PPIDE_CMD_DEVRES	0x08
#define PPIDE_CMD_RECAL		0x10
#define PPIDE_CMD_READ		0x20
#define PPIDE_CMD_WRITE		0x30
#define PPIDE_CMD_DEVDIAG	0x90
#define PPIDE_CMD_PACKET	0xA0
#define PPIDE_CMD_IDPKTDEV	0xA1
#define PPIDE_CMD_IDDEV		0xEC
#define PPIDE_CMD_SETFEAT	0xEF
//
// FEATURE BYTES
//
#define PPIDE_FEAT_ENABLE8BIT	0x01
#define PPIDE_FEAT_DISABLE8BIT	0x81
//
// PPIDE DEVICE TYPES
//
#define PPIDE_TYPEUNK	0
#define PPIDE_TYPEATA	1
#define PPIDE_TYPEATAPI	2
//
// PPIDE DEVICE STATUS CODES
//
#define PPIDE_STOK	0
#define PPIDE_STINVUNIT	-1
#define PPIDE_STNOMEDIA	-2
#define PPIDE_STCMDERR	-3
#define PPIDE_STIOERR	-4
#define PPIDE_STRDYTO	-5
#define PPIDE_STDRQTO	-6
#define PPIDE_STBSYTO	-7
#define PPIDE_STNOTSUP	-8
#define PPIDE_STNOTRDY	-9
//
// DRIVE SELECTION BYTES (FOR USE IN DRIVE/HEAD REGISTER)
//
//#define PPIDE_DRVSEL:
#define PPIDE_DRVMASTER	0xE0	// LBA,MASTER DEVICE
#define PPIDE_DRVSLAVE	0xF0	// LBA,SLAVE DEVICE

// =====================================================================
// PPIDE Port Mapping (Base 0x0244)
// =====================================================================
#define PPIDE_DATALO 0x0244    // IDE Data Bus (D0-D7)
#define PPIDE_DATAHI 0x0245    // IDE Data Bus (D8-D15)
#define PPIDE_CTL    0x0246    // IDE Control Signals
#define PPIDE_PPI    0x0247    // 82C55 Control Register

// =====================================================================
// Port C Control Bitmask Calculations (Inverted Logic)
// Reset=0,IOR=0,IOW=0,CS1=0,CS0=0 mean pins are 5V (Inactive)
// =====================================================================
#define MSK_CS0 0x08      // Pulls /CS0 Active Low (Target Command Block)
#define MSK_IOW 0x20      // Pulls /IOW Active Low (Write Strobe)
#define MSK_IOR 0x40     // Pulls /IOR Active Low (Read Strobe)

//------------------------------
//## Part 1: Selecting an LBA Sector
//To select a sector,we must write a 28-bit sector address across four 
//internal IDE registers: LBA Low,LBA Mid,LBA High,and Device/Head.
//This routine expects you to pass the 28-bit LBA address via the Z80 
//registers:

//* DE Lower 16 bits of LBA
//* H Upper 12 bits of LBA (Bits 0–7)
//* L Upper 12 bits of LBA (Bits 8–11) — Note: Only bits 0-3 are used
//here,mixed with the Master selection bit 0E0.

// =====================================================================
// ATA Command Set Definitions
// =====================================================================
#define CMD_SET_FEAT 0xEF  // ATA Set Features Command
#define FEAT_8BIT_ON 0x01   // Sub-command feature code to enable 8-bit PIO

#define SECTOR_SIZE 512

union uint32_u {
    uint32_t u32;
    uint8_t  b[4];
};

void delayn(uint16_t value);
uint8_t ideDetect(void);
void ideReset(void);
void writeReg(uint8_t REG, uint8_t DATA);
void selMst(void);
void writeWordData(uint8_t *BUFFER);
uint8_t readPulse(uint8_t CMD);
void waitReady(void);
void waitDrq(void);
void readWordData(uint8_t CMD, uint8_t *BUFFER);
void readSector(uint8_t *BUFFER);
void writeSector(uint8_t *BUFFER);
void selPio8(void);
uint32_t ideGetLba(void);
void ideInit(void);
uint16_t ideGetError(void);
void ideSetLba(uint32_t lba);
void ideReadSector(uint8_t *BUFFER);
void ideWriteSector(uint8_t *BUFFER);
void ideGetDiskInfo(uint8_t *BUFFER);
