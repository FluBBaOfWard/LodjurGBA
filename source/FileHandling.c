#include <gba.h>
#include <string.h>

#include "FileHandling.h"
#include "LNXHeader.h"
#include "Emubase.h"
#include "Main.h"
#include "Shared/EmuMenu.h"
#include "Shared/EmuSettings.h"
#include "Shared/FileHelper.h"
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
	sleepTime      = cfg.sleepTime;
	gMachineSet    = cfg.machine;
	joyCfg         = (joyCfg & ~0x400) | ((cfg.controller & 1) << 10);
//	pauseEmulation = emuSettings & AUTOPAUSE_EMULATION;
}

void updateConfigData(void) {
	strcpy(cfg.magic, "cfg");
	cfg.emuSettings   = emuSettings & ~EMUSPEED_MASK;	// Clear speed setting.
	cfg.config        = (gBorderEnable & 1) ^ 1;
	cfg.gammaValue    = gGammaValue;
	cfg.contrastValue = gContrastValue;
	cfg.sleepTime     = sleepTime;
	cfg.machine       = gMachineSet;
	cfg.controller    = (joyCfg >> 10) & 1;
}

void initSettings() {
	memset(&cfg, 0, sizeof(ConfigData));
	cfg.emuSettings   = AUTOPAUSE_EMULATION | AUTOLOAD_NVRAM | ALLOW_SPEED_HACKS;
	cfg.contrastValue = 3;
	cfg.sleepTime     = 60*60*5;
	cfg.machine       = HW_AUTO;

	applyConfigData();
}

int loadSettings() {
	bytecopy_((u8 *)&cfg, (u8 *)SRAM+0x10000-sizeof(ConfigData), sizeof(ConfigData));
	if (strstr(cfg.magic, "cfg")) {
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

	bytecopy_((u8 *)SRAM+0x10000-sizeof(ConfigData), (u8 *)&cfg, sizeof(ConfigData));
	infoOutput("Settings saved.");
}

void loadNVRAM() {
	int saveSize = 0;
	void *nvMem = NULL;
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
}

void saveNVRAM() {
	int saveSize = 0;
	void *nvMem = NULL;
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
	bytecopy_((u8 *)SRAM, nvMem, saveSize);
	infoOutput("Saved NVRAM.");*/
}

void loadState(void) {
//	unpackState(testState);
	infoOutput("Loaded state.");
}
void saveState(void) {
//	packState(testState);
	infoOutput("Saved state.");
}

//---------------------------------------------------------------------------------
void loadBioses(void) {
	const RomHeader *bh;
	int n = 0;
	while ((bh = findBios(n++))) {
		memcpy(biosSpace, (const u8 *)bh + sizeof(RomHeader), 0x200);
	}
}

bool loadGame(const RomHeader *rh) {
	if (rh) {
		gRomSize = rh->filesize;
		romSpacePtr = (const u8 *)rh + sizeof(RomHeader);
		selectedGame = selected;
		setPowerIsOn(false);
		if ((gHasHeader = checkLnxHeader((LnxHeader *)romSpacePtr))) {
			gRomSize = lnxHeader.bank0PageSize << 8;
		}
		checkMachine(rh);
//		setEmuSpeed(0);
		loadCart();
		gameInserted = true;
		if (emuSettings & AUTOLOAD_NVRAM) {
			loadNVRAM();
		}
		if (emuSettings & AUTOLOAD_STATE) {
			loadState();
		}
		setPowerIsOn(true);
		closeMenu();
		return false;
	}
	return true;
}

void selectGame() {
	pauseEmulation = true;
	ui9();
	const RomHeader *rh = browseForFile();
	if (loadGame(rh)) {
		backOutOfMenu();
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

void checkMachine(const RomHeader *rh) {
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
