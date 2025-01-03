#ifndef CART_HEADER
#define CART_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#include "LynxCart/LynxCart.h"

extern u32 gRomSize;
extern u32 maxRomSize;
extern u8 gConfig;
extern u8 gMachineSet;
extern u8 gMachine;
extern u8 gSOC;
extern u8 gLang;
extern u8 gPaletteBank;
extern u8 gGameID;

extern u8 lynxRAM[0x10000];
extern u8 biosSpace[0x200];
extern const u8 *romSpacePtr;

extern CART cart_0;

void machineInit(void);
void loadCart(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // CART_HEADER
