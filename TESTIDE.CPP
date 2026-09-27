//TAB=4
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>
#include <conio.h>

#include "IDE.H"

//#define TEST_IDE_INIT
#define TEST_GETDISKINFO

uint8_t		sector_buffer[SECTOR_SIZE];
uint32_t	LBA, LBA_R;
uint32_t	TOTAL_NB_SECT;

uint32_t getDouble(uint8_t *buffer);
void clearSectBuff(void);
void mem_dump(uint16_t address);
void GetAnyKey(void);

int main(void)
{
uint16_t error;

	LBA = 0L;

	printf("ideDetect: %02X\r\n",ideDetect());

#ifdef TEST_IDE_INIT
	printf("ideReset\r\n");
	ideReset();
	printf("waitReady\r\n");
	waitReady();

	printf("selMst\r\n");
	selMst();
	printf("waitReady 2\r\n");
	waitReady();
	printf("selPio8\r\n");
	selPio8();
	waitReady();
#else
	ideInit();
#endif

	printf("check Master\r\n");
	printf("MST/LBA: %02X\r\n",ideGetLba()>>24);

	error = ideGetError();
	printf("checkError: %04X\r\n",error);

	printf("test set/get LBA\r\n");

	LBA = 0x00123456L;
	printf("set LBA\r\n");
	ideSetLba(LBA);

	printf("get LBA\r\n");
	LBA_R = ideGetLba();

	printf("LBA: %08lX: %08lX\r\n",LBA,ideGetLba());

	// Word Offset		Byte Offset		Field Description	Format
	// Words 10–19		0x14 – 0x27		Serial Number		20-character ASCII string
	// Words 27–46		0x36 – 0x5D		Model Number/Name	40-character ASCII string
	// Words 60–61		0x78 – 0x7B		Total LBA Sectors	32-bit Integer (Capacity)

	LBA = 0L;
	ideSetLba(LBA);

// FIXME 20260925 GetDiskInfo not working	
#ifdef TEST_GETDISKINFO	
	clearSectBuff();
	waitReady();
	ideGetDiskInfo(sector_buffer);

	//TOTAL_NB_SECT = *((uint32_t *)(&sector_buffer[0x78]));

	TOTAL_NB_SECT = getDouble(&sector_buffer[0x78]);

	printf("Serial Number: %s\r\n",&sector_buffer[0x14]);
	printf("Model Nb/name: %s\r\n",&sector_buffer[0x36]);
	printf("Total LBA sec: %08lX\r\n",TOTAL_NB_SECT);

	GetAnyKey();
#endif
	printf("ID sector\r\n");
	mem_dump(0);
	GetAnyKey();

	printf("Dump sectors\r\n");
	LBA = 0L;

	do
	{
		ideSetLba(LBA);
		ideReadSector(sector_buffer);

		mem_dump(0);
		GetAnyKey();
		mem_dump(0x0100);
		GetAnyKey();
		LBA++;
	} while (true);

	return 0;
}

uint32_t getDouble(uint8_t *buffer)
{
union uint32_u val;

	val.b[0] = *buffer;
	val.b[1] = *(buffer+1);
	val.b[2] = *(buffer+2);
	val.b[3] = *(buffer+3);

	return val.u32;
}

void GetAnyKey(void)
{
	printf("\r\nPress any key to continue\r\n");
	getch();
}

void clearSectBuff(void)
{
uint16_t i = 0;

	do
	{
		sector_buffer[i] = 0;
	} while (i++ != SECTOR_SIZE);
}

void mem_dump(uint16_t address)
{
	uint8_t a,b;
	uint16_t tmp;

	tmp = address & 0xFF00;

	printf("      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\r\n");

    do
    {
		printf("%04X ",tmp);

		b = 16;
		do
		{
			printf("%02X ",sector_buffer[tmp++]);
		} while (--b != 0);

		tmp -= 16;

		b = 16;
		do
		{
			a = sector_buffer[tmp++];

			if ((a < 0x20) || (a > 0x7F)) a = '.';
			putchar(a);
		} while (--b != 0);

		printf("\r\n");

    } while ((tmp & 0x00f0) != 0);
}


