#include <gba.h>
#include <string.h>

#include "FileHandling.h"
#include "LNXHeader.h"
#include "Emubase.h"
#include "Main.h"
#include "Shared/EmuMenu.h"
#include "Shared/EmuSettings.h"
#include "Shared/FileHelper.h"
#include "Shared/SRAMHandler.h"
#include "Shared/AsmExtra.h"
#include "Gui.h"
#include "Cart.h"
#include "cpu.h"
#include "io.h"

/// Used for emulators or flashcarts to choose save type.
const char *const sramTag = "SRAM_Vnnn";

static bool checkLnxHeader(const LnxHeader *rHead);

EWRAM_BSS int selectedGame = 0;
EWRAM_BSS ConfigData cfg;
EWRAM_BSS LnxHeader lnxHeader;

//---------------------------------------------------------------------------------
void applyConfigData(void) {
	emuSettings    = cfg.emuSettings & ~EMUSPEED_MASK; // Clear speed setting.
	gBorderEnable  = (cfg.config & 1) ^ 1;
	gGammaValue    = cfg.gammaValue;
	gContrastValue = cfg.contrastValue;
	gMachineSet    = cfg.machine;
	joyCfg         = (joyCfg & ~0x400) | ((cfg.controller & 1) << 10);
//	pauseEmulation = emuSettings & AUTOPAUSE_EMULATION;
	setSleepValue(emuSettings & AUTOSLEEP_MASK);
}

void updateConfigData(void) {
	cfg.emuSettings   = emuSettings & ~EMUSPEED_MASK;	// Clear speed setting.
	cfg.config        = (gBorderEnable & 1) ^ 1;
	cfg.gammaValue    = gGammaValue;
	cfg.contrastValue = gContrastValue;
	cfg.machine       = gMachineSet;
	cfg.controller    = (joyCfg >> 10) & 1;
}

void initSettings() {
	memset(&cfg, 0, sizeof(ConfigData));
	cfg.emuSettings   = AUTOPAUSE_EMULATION | AUTOLOAD_NVRAM | AUTOSLEEP_OFF | ALLOW_SPEED_HACKS;
	cfg.contrastValue = 1;
	cfg.machine       = HW_AUTO;

	applyConfigData();
}

int loadSettings() {
	if (readFile((u8 *)&cfg, sizeof(cfg), LNXID)) {
		applyConfigData();
		infoOutput("Settings loaded.");
		return 0;
	}
	else {
		updateConfigData();
		infoOutput("Error in settings file.");
	}
	return 1;
}
void saveSettings() {
	updateConfigData();

	if (writeFile((u8 *)&cfg, sizeof(cfg), LNXID, "Config")) {
		infoOutput("Settings saved.");
	}
	else {
		infoOutput("Could not save settings file.");
	}
}

int loadNVRAM() {
//	int saveSize = 0;
//	void *nvMem = NULL;
/*
	if (sramSize > 0) {
		saveSize = sizeof(wsSRAM);
		nvMem = wsSRAM;
	}
	else if (eepromSize > 0) {
		saveSize = eepromSize;
		nvMem = extEepromMem;
	}
	else {
		return;
	}
	bytecopy_(nvMem, (u8 *)SRAM, saveSize);
	infoOutput("Loaded NVRAM.");*/
	return 0;
}

void saveNVRAM() {
//	if (gCartFlags & SRAMFLAG) {
//		forceSaveNVRAM();
//	}
}
int forceSaveNVRAMChk() {
//	if (saveEmuSram(EMU_SRAM, 0x8000)) {
//		infoOutput("Saved NVRAM.");
//		return 1;
//	}
	return 0;
}
void forceSaveNVRAM() {
	forceSaveNVRAMChk();
}

int loadStateChk() {
	if (getStateSize() < 0x10000
		&& quickLoad()) {
		infoOutput("Loaded State.");
		return loadNVRAM();
	}
	return 0;
}
int saveStateChk() {
	if (getStateSize() < 0x10000
		&& quickSave()) {
		infoOutput("Saved State.");
		return forceSaveNVRAMChk();
	}
	return 0;
}

void loadState() {
	loadStateChk();
}
void saveState() {
	saveStateChk();
}

//---------------------------------------------------------------------------------
bool loadGame(const RomHeader *rh) {
	if (rh) {
		return loadROM(rh->romData, rh->filesize, rh->flags);
	}
	return true;
}
//---------------------------------------------------------------------------------
bool loadROM(const u8 *rom, int size, int emuFlags) {
	selectedGame = selected;
	gRomSize = size;
	romSpacePtr = rom;
	setPowerIsOn(false);
	if ((gHasHeader = checkLnxHeader((LnxHeader *)romSpacePtr))) {
		gRomSize = lnxHeader.bank0PageSize << 8;
	}
	checkMachine();
//	setEmuSpeed(0);
	loadCart();
	int loadedSRAM = 0;
	if (emuSettings & AUTOLOAD_STATE) {
		loadedSRAM = loadStateChk();
	}
	if ((emuSettings & AUTOLOAD_NVRAM)
		&& loadedSRAM == 0) {
		loadNVRAM();
	}
	gameInserted = true;
	setPowerIsOn(true);
	closeMenu();
	return false;
}

void selectGame() {
	pauseEmulation = true;
	ui9();
	const RomHeader *rh = browseForFile();
	if (loadGame(rh)) {
		backOutOfMenu();
	}
}

//---------------------------------------------------------------------------------
void loadBioses(void) {
	const RomHeader *bh;
	int n = 0;
	while ((bh = findBios(n++))) {
		memcpy(biosSpace, (const u8 *)bh + sizeof(RomHeader), 0x200);
	}
}

bool checkLnxHeader(const LnxHeader *lHead) {
	bool isLNX = false;
	if (lHead->magic[0] == 'L'
			&& lHead->magic[1] == 'Y'
			&& lHead->magic[2] == 'N'
			&& lHead->magic[3] == 'X'
			&& lHead->versionNumber == 1) {
		isLNX = true;
		memcpy(&lnxHeader, lHead, sizeof(LnxHeader));
	}
	else {
		memset(&lnxHeader, 0, sizeof(LnxHeader));
	}
	int smRot = gScreenMode;
	int headRot = lnxHeader.rotation;
	if (headRot == 1 || headRot == 2) {
		smRot = headRot;
	}
	gRotation = smRot;
	setScreenMode(smRot);
	return isLNX;
}

void checkMachine() {
	u8 newMachine = gMachineSet;
	if (newMachine == HW_AUTO) {
		newMachine = HW_LYNX_II;
	}
	if (gMachine != newMachine) {
		gMachine = newMachine;
		cpuInit(gMachine);
		setupEmuBackground();
	}
}
