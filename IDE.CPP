#include "IDE.H"

void delayn(uint16_t value)
{
	asm{
		PUSH    CX
		MOV		CX,value
		PUSH    CX
		MOV     CX,236
		NOP
		DB		0E2h, 0FDh
		POP	    CX
		DB		0E2h, 0F6h
		POP	    CX
	}
}

uint8_t ideDetect(void)
{
	asm{
		MOV	    AL,PPIDE_DIR_WRITE
		MOV	    DX,PPIDE_PPI
		OUT	    (DX),AL
		MOV	    AL,0A5h
		MOV	    DX,PPIDE_DATALO
		OUT	    (DX),AL
		IN		AL,(DX)
		XOR	    AL,0A5h
		NOT	    AL
	}
	return _AL;
}

void ideReset(void)
{
	asm{
		MOV	     AL,PPIDE_DIR_WRITE
		MOV	     DX,PPIDE_PPI
		OUT	     (DX),AL

		MOV	     AL,PPIDE_CTL_RESET
		MOV	     DX,PPIDE_CTL
		OUT	     (DX),AL

		PUSH    CX
		MOV     CX,10
		NOP
		DB  	0E2h, 0FDh
		POP	    CX

		XOR		AL,AL
		OUT		(DX),AL
	}
}

void writeReg(uint8_t REG, uint8_t DATA)
{
	asm{
		MOV	     AL,PPIDE_DIR_WRITE
		MOV	     DX,PPIDE_PPI
		OUT	     (DX),AL

		MOV		AL,DATA
		MOV		DX,PPIDE_DATALO
		OUT		(DX),AL

		MOV     AL,REG
		MOV		DX,PPIDE_CTL
		OUT		(DX),AL
		OR		AL,MSK_IOW
		OUT		(DX),AL
		XOR		AL,AL
		OUT		(DX),AL
	}
}

void selMst(void)
{
	writeReg(CTL_MAS_LBA,PPIDE_DRVMASTER);
}

void writeWordData(uint8_t *BUFFER)
{
	asm{
		MOV		DI,BUFFER
		MOV	    AL,PPIDE_DIR_WRITE
		MOV	    DX,PPIDE_PPI
		OUT	    (DX),AL

		MOV		AL,[DI]
		INC		DI
		MOV		DX,PPIDE_DATALO
		OUT		DX,AL
		MOV		AL,[DI]
		INC     DI
		MOV		DX,PPIDE_DATAHI
		OUT		DX,AL
		MOV     AL,CTL_BYTE

		MOV		DX,PPIDE_CTL
		OUT		(DX),AL
		OR		AL,MSK_IOW
		OUT		(DX),AL
		XOR		AL,AL
		OUT		(DX),AL
	}
}

uint8_t readPulse(uint8_t CMD)
{
	asm{
		MOV		AL,CMD
		MOV		DX,PPIDE_CTL
		OUT		(DX),AL
		OR		AL,MSK_IOR
		OUT		(DX),AL
		MOV		DX,PPIDE_DATALO
		IN      AL,(DX)
		MOV		AH,AL
		XOR		AL,AL
		MOV		DX,PPIDE_CTL
		OUT		(DX),AL
		MOV		AL,AH
	}
	return _AL;
}

void waitReady(void)
{
uint8_t status;

	asm{
		MOV	    AL,PPIDE_DIR_READ
		MOV	    DX,PPIDE_PPI
		OUT	    (DX),AL
	}

	do {
		status = readPulse(CTL_STATUS);
	} while ((status & 0xC0) != 0x40);
}

void waitDrq(void)
{
uint8_t status;

	asm{
		MOV	    AL,PPIDE_DIR_READ
		MOV	    DX,PPIDE_PPI
		OUT	    (DX),AL
	}

	do {
		status = readPulse(CTL_STATUS);
	} while ((status & 0x08) == 0);
}

void readWordData(uint8_t CMD, uint8_t *BUFFER)
{
	asm{
		MOV		DI,BUFFER
		MOV		AL,CMD
		MOV		DX,PPIDE_CTL
		OUT		(DX),AL
		OR		AL,MSK_IOR
		OUT		(DX),AL
		MOV		DX,PPIDE_DATALO
		IN      AL,(DX)
		MOV     [DI],AL
		INC     DI
		MOV		DX,PPIDE_DATAHI
		IN      AL,(DX)
		MOV     [DI],AL
		MOV		DX,PPIDE_CTL
		XOR		AL,AL
		OUT		(DX),AL
	}
}

void readSector(uint8_t *BUFFER)
{
uint8_t b = 0;
	do
	{
		readWordData(CTL_PRELOAD,BUFFER);
		BUFFER += 2;
	} while (--b != 0);
}

void writeSector(uint8_t *BUFFER)
{
uint8_t b = 0;
	do
	{
		writeWordData(BUFFER);
		BUFFER += 2;
	} while (--b != 0);
}

void selPio8(void)
{
	writeReg(CTL_FEAT,FEAT_8BIT_ON);
	writeReg(CTL_CMD,CMD_SET_FEAT);
}

uint32_t ideGetLba(void)
{
	uint32_t temp;
	
	asm{
		MOV	    AL,PPIDE_DIR_READ
		MOV	    DX,PPIDE_PPI
		OUT	    (DX),AL
	}

	temp = (uint32_t) readPulse(CTL_MAS_LBA);
	temp = (temp << 8) | (uint32_t) readPulse(CTL_LBA_HIG);
	temp = (temp << 8) | (uint32_t) readPulse(CTL_LBA_MID);
	temp = (temp << 8) | (uint32_t) readPulse(CTL_LBA_LOW);

	return temp;
}

void ideInit(void)
{
	ideReset();
	waitReady();
	selMst();
	waitReady();
	selPio8();
	waitReady();
}

uint16_t ideGetError(void)
{
uint8_t result;

	asm{
		MOV	    AL,PPIDE_DIR_READ
		MOV	    DX,PPIDE_PPI
		OUT	    (DX),AL
	}

	result = readPulse(CTL_STATUS);

	if ((result & 0x01) == 0) return result;

	asm{
		MOV	    AL,PPIDE_DIR_READ
		MOV	    DX,PPIDE_PPI
		OUT	    (DX),AL
	}

	return 0xFF00 | readPulse(CTL_ERROR);
}

void ideSetLba(uint32_t lba)
{
	//waitReady();

	asm{
		MOV	    AL,PPIDE_DIR_WRITE
		MOV	    DX,PPIDE_PPI
		OUT	    (DX),AL
	}

	writeReg(CTL_MAS_LBA,((lba >> 24) & 0x0F) | 0xE0);
	writeReg(CTL_LBA_HIG,(lba >> 16) & 0xFF);
	writeReg(CTL_LBA_MID,(lba >> 8) & 0xFF);
	writeReg(CTL_LBA_LOW,lba & 0xFF);
	writeReg(CTL_DRHD,1);
}

void ideReadSector(uint8_t *BUFFER)
{
	waitReady();
	writeReg(CTL_CMD,PPIDE_CMD_READ);
	waitDrq();
	readSector(BUFFER);
}

void ideWriteSector(uint8_t *BUFFER)
{
	writeReg(CTL_CMD,PPIDE_CMD_WRITE);
	waitDrq();

	asm{
		MOV	    AL,PPIDE_DIR_WRITE
		MOV	    DX,PPIDE_PPI
		OUT	    (DX),AL
	}

	writeSector(BUFFER);
	waitReady();
}

void ideGetDiskInfo(uint8_t *BUFFER)
{
	writeReg(CTL_CMD,PPIDE_CMD_IDDEV);

	asm{
		MOV	    AL,PPIDE_DIR_READ
		MOV	    DX,PPIDE_PPI
		OUT	    (DX),AL
	}

	waitDrq();
	readSector(BUFFER);
}
