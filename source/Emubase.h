#ifndef EMUBASE
#define EMUBASE

#ifdef __cplusplus
extern "C" {
#endif

#define LNXID 0x1A584C41		// "ALX",0x1A - Lynx

#define HALF_CPU_SPEED		(1<<16)
#define ALLOW_SPEED_HACKS	(1<<17)
#define ENABLE_HEADPHONES	(1<<18)

typedef struct {
	const u32 identifier;		// LNXID
	const u32 filesize;
	const u32 flags;			// Bit 1 = LynxII.
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
	u8 contrastValue;			// from gfx.s
	u8 machine;
	char currentPath[256];
} ConfigData;

#ifdef __cplusplus
} // extern "C"
#endif

#endif // EMUBASE
