#ifndef GUI_HEADER
#define GUI_HEADER

#ifdef __cplusplus
extern "C" {
#endif

#define HALF_CPU_SPEED		(1<<16)
#define ALLOW_SPEED_HACKS	(1<<17)
#define ENABLE_HEADPHONES	(1<<18)

extern u8 gGammaValue;
extern u8 gContrastValue;
extern u8 gBorderEnable;
extern u8 gScreenMode;
extern u8 gRotation;

void setupGUI(void);
void enterGUI(void);
void exitGUI(void);
void quickSelectGame(void);
void nullUINormal(int key);
void nullUIDebug(int key);
void resetGame(void);

void uiNullNormal(void);
void uiMainMenu(void);
void uiFile(void);
void uiAbout(void);
void uiLoadGame(void);

void debugIOUnmappedR(u16 port);
void debugIOUnmappedW(u8 val, u16 port);
void debugIOUnimplR(u16 port);
void debugIOUnimplW(u8 val, u16 port);
void debugDivideError(void);
void debugUndefinedInstruction(void);
void debugCrashInstruction(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // GUI_HEADER
