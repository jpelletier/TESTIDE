# TESTIDE
This is a program running on DOS on a PC XT for a PPIDE ISA card.

The software verify the lowest level software/hardware functionality:
-reset the hard drive
-set the master disk
-set the PIO8 mode
-check the master bit set
-check set/get LBA
-get disk info and display
    -Serial Number
	-Model Number/Name
	-Total # LBA Sectors
-dump sectors half by half (256 bytes for 512-byte sectors)

The IO port for the PPIDE port is 0x0244 (IDE.H line 65)

To compile, I used Borland turbo C++ v3 running on DOSEMU2
