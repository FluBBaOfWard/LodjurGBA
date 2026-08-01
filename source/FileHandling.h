#ifndef FILEHANDLING_HEADER
#define FILEHANDLING_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#include "Emubase.h"
#include "Lynx.h"

#define FILEEXTENSIONS ".lnx.lyx.bin.o"

extern ConfigData cfg;

void initSettings(void);
int loadSettings(void);
void saveSettings(void);
bool loadROM(const u8 *rom, int size, int emuFlags);
bool loadGame(const RomHeader *rh);
void checkMachine();
int loadNVRAM(void);
void saveNVRAM(void);
void forceSaveNVRAM(void);
void loadState(void);
void saveState(void);
void selectGame(void);
void loadBioses(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // !FILEHANDLING_HEADER
