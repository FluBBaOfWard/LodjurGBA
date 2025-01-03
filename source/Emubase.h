#ifndef EMUBASE
#define EMUBASE

#ifdef __cplusplus
extern "C" {
#endif

#define LNXID 0x1A535742		// "BWS",0x1A - Lynx

typedef struct {
	const u32 identifier;		// LNXID
	const u32 filesize;
	const u32 flags;			// Bit 1 = PCV2, Bit 2 = WSC, Bit 3 = SwanCrystal.
	const u32 undefined;
	const u8 bios;				// Bit 0 = Bios,
	const u8 reserved[15];
	const char name[32];
} RomHeader;

typedef struct {				//(config struct)
	char magic[4];				//="CFG",0
	int emuSettings;
	int sleepTime;				// autoSleepTime in frames
	u8 gammaValue;				// from gfx.s
	u8 config;					// from cart.s
	u8 controller;				// from io.s
	u8 language;
	u8 palette;
	char currentPath[256];
} ConfigData;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // EMUBASE
