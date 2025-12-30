#include <gba.h>
#include <string.h>

#include "Gui.h"
#include "Shared/EmuMenu.h"
#include "Shared/EmuSettings.h"
#include "Main.h"
#include "FileHandling.h"
#include "Lynx.h"
#include "Cart.h"
#include "Gfx.h"
#include "Sound.h"
#include "io.h"
#include "cpu.h"
#include "ARMMikey/ARM6502/Version.h"
#include "ARMMikey/Version.h"
#include "ARMSuzy/Version.h"

#define EMUVERSION "V0.1.4 2025-12-30"

void hacksInit(void);

static void gammaChange(void);
static void machineSet(void);
static const char *getMachineText(void);
static void speedHackSet(void);
static const char *getSpeedHackText(void);
static void cpuHalfSet(void);
static void borderSet(void);
static const char *getBorderText(void);
static void soundSet(void);
static void stepFrameUI(void);
//static const char *getControllerText(void);
//static const char *getJoyMappingText(void);
static void swapABSet(void);
static const char *getSwapABText(void);
static void contrastSet(void);
static const char *getContrastText(void);
static void screenModeSet(void);
static const char *getScreenModeText(void);

const MItem dummyItems[] = {
	{"", uiDummy},
};
const MItem mainItems[] = {
	{"File->", ui2},
	{"Controller->", ui3},
	{"Display->", ui4},
	{"Settings->", ui5},
	{"Machine->", ui6},
	{"Debug->", ui7},
	{"About->", ui8},
	{"Sleep", gbaSleep},
	{"Reset Console", resetGame},
	{"Quit Emulator", ui10},
};
const MItem fileItems[] = {
	{"Load Game->", selectGame},
	{"Load NVRAM", loadNVRAM},
	{"Save NVRAM", saveNVRAM},
	{"Save Settings", saveSettings},
	{"Reset Game", resetGame},
};
const MItem ctrlItems[] = {
	{"B Autofire: ", autoBSet, getAutoBText},
	{"A Autofire: ", autoASet, getAutoAText},
	{"Swap A-B:   ", swapABSet, getSwapABText},
};
const MItem displayItems[] = {
	{"Screen:", screenModeSet, getScreenModeText},
	{"Gamma: ", gammaChange, getGammaText},
	{"Contrast: ", contrastSet, getContrastText},
	{"Border: ", borderSet, getBorderText},
};
const MItem setItems[] = {
	{"Speed: ", speedSet, getSpeedText},
	{"Autoload State: ", autoStateSet, getAutoStateText},
	{"Autosave Settings: ", autoSettingsSet, getAutoSettingsText},
	{"Autopause Game: ", autoPauseGameSet, getAutoPauseGameText},
	{"EWRAM Overclock: ", ewramSet, getEWRAMText},
	{"Autosleep: ", sleepSet, getSleepText},
};
const MItem machineItems[] = {
	{"Machine: ", machineSet, getMachineText},
	{"Cpu Speed Hacks: ", speedHackSet, getSpeedHackText},
	{"Half Cpu Speed: ", cpuHalfSet},
	{"Sound: ", soundSet, getSoundEnableText},
	//{"", languageSet},
};
const MItem debugItems[] = {
	{"Debug Output:", debugTextSet, getDebugText},
	{"Step Frame", stepFrameUI},
};
const MItem fnList9[] = {
	{"", quickSelectGame},
};
const MItem quitItems[] = {
	{"Yes", exitEmulator},
	{"No", backOutOfMenu},
};

const Menu menu0 = MENU_M("", uiNullNormal, dummyItems);
Menu menu1 = MENU_M("Main Menu", uiAuto, mainItems);
const Menu menu2 = MENU_M("File Handling", uiAuto, fileItems);
const Menu menu3 = MENU_M("Controller Settings", uiAuto, ctrlItems);
const Menu menu4 = MENU_M("Display Settings", uiAuto, displayItems);
const Menu menu5 = MENU_M("Other Settings", uiAuto, setItems);
const Menu menu6 = MENU_M("Machine Settings", uiAuto, machineItems);
const Menu menu7 = MENU_M("Debug", uiAuto, debugItems);
const Menu menu8 = MENU_M("About", uiAbout, dummyItems);
const Menu menu9 = MENU_M("Load game", uiLoadGame, fnList9);
const Menu menu10 = MENU_M("Quit Emulator?", uiAuto, quitItems);

const Menu *const menus[] = {&menu0, &menu1, &menu2, &menu3, &menu4, &menu5, &menu6, &menu7, &menu8, &menu9, &menu10 };

u8 gGammaValue = 0;
u8 gContrastValue = 3;
u8 gBorderEnable = 1;
u8 gScreenMode = 0;
u8 gRotation = 0;

const char *const machTxt[]  = {"Auto", "Lynx", "Lynx II"};
const char *const bordTxt[]  = {"Black", "Frame", "BG Color", "None"};
const char *const scrModeTxt[]  = {"1:1", "Rot Left", "Rot Right", "Zoom"};

/// This is called at the start of the emulator
void setupGUI() {
//	keysSetRepeat(25, 4);	// Delay, repeat.
	menu1.itemCount = ARRSIZE(mainItems) - (enableExit?0:1);
	closeMenu();
}

/// This is called when going from emu to ui.
void enterGUI() {
	GFX_DISPCNT = MODE_0
				| OBJ_1D_MAP
				| BG0_ON
				| BG1_ON
				| BG2_ON
				| BG3_ON
				| OBJ_ON
				| WIN0_ON
				| WIN1_ON
				;
}

/// This is called going from ui to emu.
void exitGUI() {
	setupEmuBorderPalette();
	GFX_DISPCNT = MODE_3
				| BG2_ON
//				| WIN0_ON
//				| WIN1_ON
				;
}

void quickSelectGame() {
	openMenu();
	selectGame();
	closeMenu();
}

void uiNullNormal() {
	uiNullDefault();
}

void uiAbout() {
	setupSubMenuText();
	drawText("B:         Lynx B Button", 3);
	drawText("A:         Lynx A Button", 4);
	drawText("L:         Lynx Option I", 5);
	drawText("R:         Lynx Option II", 6);
	drawText("Start:     Lynx Pause Button", 7);
	drawText("DPad:      Lynx d-pad", 8);

	drawText("LodjurGBA  " EMUVERSION, 16);
	drawText("ARMSuzy    " ARMSUZYVERSION, 17);
	drawText("ARMMIKEY   " ARMMIKEYVERSION, 18);
	drawText("ARM6502    " ARM6502VERSION, 19);
}

void uiLoadGame() {
	setupSubMenuText();
}

void nullUINormal(int key) {
}

void nullUIDebug(int key) {
}

void resetGame() {
	loadCart();
	setupEmuBackground();
	setupMenuPalette();
	powerIsOn = true;
}

//---------------------------------------------------------------------------------
void debugIO(u16 port, u8 val, const char *message) {
	char debugString[32];

	debugString[0] = 0;
	strlcat(debugString, message, sizeof(debugString));
	short2HexStr(&debugString[strlen(debugString)], port);
	strlcat(debugString, " val:", sizeof(debugString));
	char2HexStr(&debugString[strlen(debugString)], val);
	debugOutput(debugString);
}
//---------------------------------------------------------------------------------
void debugIOUnimplR(u16 port) {
	debugIO(port, 0, "Unimpl R port:");
}
void debugIOUnimplW(u8 val, u16 port) {
	debugIO(port, val, "Unimpl W port:");
}
void debugIOUnmappedR(u16 port) {
	debugIO(port, 0, "Unmapped R port:");
}
void debugIOUnmappedW(u8 val, u16 port) {
	debugIO(port, val, "Unmapped W port:");
}
void debugIOMirroredR(u16 port) {
	debugIO(port, 0, "Mirrored R port:");
}
void debugIOMirroredW(u16 port, u8 val) {
	debugIO(port, val, "Mirrored W port:");
}
void debugDivideError() {
	debugOutput("Divide Error.");
}
void debugUndefinedInstruction() {
	debugOutput("Undefined Instruction.");
}
void debugPowerOff() {
	debugOutput("Power Off!");
}

void stepFrameUI() {
	stepFrame();
	setupMenuPalette();
}
//---------------------------------------------------------------------------------
/// Swap A & B buttons
void swapABSet() {
	joyCfg ^= 0x400;
}
const char *getSwapABText() {
	return autoTxt[(joyCfg>>10)&1];
}

/// Change gamma (brightness)
void gammaChange() {
	gammaSet();
	paletteInit(gGammaValue, gContrastValue);
	paletteTxAll();					// Make new palette visible
//	setupEmuBorderPalette();
	setupMenuPalette();
}

/// Change contrast
void contrastSet() {
	gContrastValue++;
	if (gContrastValue > 4) gContrastValue = 0;
	paletteInit(gGammaValue, gContrastValue);
	paletteTxAll();					// Make new palette visible
//	setupEmuBorderPalette();
	setupMenuPalette();
	settingsChanged = true;
}
const char *getContrastText() {
	return brighTxt[gContrastValue];
}

/// Change screen mode
void screenModeSet() {
	gScreenMode++;
	if (gScreenMode > 3) gScreenMode = 0;
	gRotation = gScreenMode;
	setScreenMode(gScreenMode);
	settingsChanged = true;
}
const char *getScreenModeText() {
	return scrModeTxt[gScreenMode];
}

void borderSet() {
	gBorderEnable ^= 0x01;
	setupEmuBorderPalette();
	setupMenuPalette();
}
const char *getBorderText() {
	return bordTxt[gBorderEnable];
}

void machineSet() {
	gMachineSet++;
	if (gMachineSet >= HW_SELECT_END) {
		gMachineSet = 0;
	}
}
const char *getMachineText() {
	return machTxt[gMachineSet];
}

void speedHackSet() {
	emuSettings ^= ALLOW_SPEED_HACKS;
	emuSettings &= ~HALF_CPU_SPEED;
//	hacksInit();
}
const char *getSpeedHackText() {
	return autoTxt[(emuSettings & ALLOW_SPEED_HACKS)>>17];
}
void cpuHalfSet() {
	emuSettings ^= HALF_CPU_SPEED;
	emuSettings &= ~ALLOW_SPEED_HACKS;
//	tweakCpuSpeed(emuSettings & HALF_CPU_SPEED);
}

void soundSet() {
	soundEnableSet();
	soundMode = (emuSettings & SOUND_ENABLE)>>10;
	soundInit();
}
