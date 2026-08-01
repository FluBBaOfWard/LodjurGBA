//
//  Sound.h
//  LodjurGBA
//
//  Created by Fredrik Ahlström on 2024-07-29.
//  Copyright © 2024-2026 Fredrik Ahlström. All rights reserved.
//
#ifndef SOUND_HEADER
#define SOUND_HEADER

#ifdef __cplusplus
extern "C" {
#endif

extern u8 soundMode;

void soundInit(void);
void soundSetMuteGUI(void);
void soundSwapBuffers(void);
void soundRender(void);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // !SOUND_HEADER
