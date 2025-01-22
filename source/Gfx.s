#ifdef __arm__

#include "Shared/gba_asm.h"
#include "ARMSuzy/ARMSuzy.i"
#include "ARMMikey/ARMMikey.i"

	.global gFlicker
	.global gTwitch
	.global gGfxMask
	.global GFX_DISPCNT
	.global GFX_BG0CNT
	.global GFX_BG1CNT
	.global EMUPALBUFF
	.global frameTotal
	.global suzy_0

	.global gfxInit
	.global gfxReset
	.global paletteInit
	.global paletteTxAll
	.global gfxRefresh
	.global gfxEndFrame
	.global vblIrqHandler
	.global lnxSuzySetButtonData
	.global updateLCDRefresh
	.global setScreenRefresh
	.global lowerRefresh

	.syntax unified
	.arm

#ifdef GBA
	.section .ewram, "ax", %progbits	;@ For the GBA
#else
	.section .text						;@ For anything else
#endif
	.align 2
;@----------------------------------------------------------------------------
gfxInit:					;@ Called from machineInit
;@----------------------------------------------------------------------------
	stmfd sp!,{lr}

	bl gfxWinInit

	ldmfd sp!,{pc}

;@----------------------------------------------------------------------------
gfxReset:					;@ Called with CPU reset
;@----------------------------------------------------------------------------
	stmfd sp!,{lr}

	ldr r0,=gfxState
	mov r1,#5					;@ 5*4
	bl memclr_					;@ Clear GFX regs

	ldr r0,=(((SCREEN_HEIGHT-GAME_HEIGHT)/2) * SCREEN_WIDTH * 2)
	add r0,r0,#SCREEN_WIDTH-GAME_WIDTH
	add r0,r0,#0x06000000
	str r0,currentDest

	bl gfxWinInit

	ldr r0,=lodjurRenderCallback
	ldr r1,=gfxEndFrame
	ldr r2,=lynxRAM
	ldr r3,=gSOC
	ldrb r3,[r3]
	bl mikeyReset

	ldr r0,=lynxRAM
	bl suzyReset0

	ldr r0,=gGammaValue
	ldr r1,=gContrastValue
	ldrb r0,[r0]
	ldrb r1,[r1]
	bl paletteInit				;@ Do palette mapping
	bl paletteTxAll				;@ Transfer it

	ldmfd sp!,{pc}

;@----------------------------------------------------------------------------
gfxWinInit:
;@----------------------------------------------------------------------------
	mov r0,#REG_BASE
	;@ Horizontal start-end
	ldr r1,=(((SCREEN_WIDTH-GAME_WIDTH)/2)<<8)+(SCREEN_WIDTH+GAME_WIDTH)/2
	orr r1,r1,r1,lsl#16			;@ Also WIN1H
	str r1,[r0,#REG_WIN0H]
	;@ Vertical start-end
	ldr r2,=(((SCREEN_HEIGHT-GAME_HEIGHT)/2)<<8)+(SCREEN_HEIGHT+GAME_HEIGHT)/2
	orr r2,r2,r2,lsl#16			;@ Also WIN1V
	str r2,[r0,#REG_WIN0V]

	ldr r3,=0x002C3B3B			;@ WinIN0/1, BG0, BG1, BG3, SPR & COL inside Win0
	str r3,[r0,#REG_WININ]		;@ WinOUT, Only BG2, BG3 & COL enabled outside Windows.
	bx lr
;@----------------------------------------------------------------------------
paletteInit:		;@ r0-r3 modified.
	.type paletteInit STT_FUNC
;@ Called by ui.c:  void paletteInit(gammaVal, contrast);
;@----------------------------------------------------------------------------
	stmfd sp!,{r4-r8,lr}
	mov r8,#30
	rsb r1,r1,#4
	mul r8,r1,r8
	mov r1,r0					;@ Gamma value = 0 -> 4
	mov r7,#0xF					;@ mask
	ldr r6,=MAPPED_RGB
	mov r4,#4096*2
	sub r4,r4,#2
noMap:							;@ Map 0000ggggrrrrbbbb  ->  0bbbbbgggggrrrrr
	and r0,r7,r4,lsr#5			;@ Blue ready
	bl gPrefix
	mov r5,r0,lsl#10

	and r0,r7,r4,lsr#9			;@ Green ready
	bl gPrefix
	orr r5,r5,r0,lsl#5

	and r0,r7,r4,lsr#1			;@ Red ready
	bl gPrefix
	orr r5,r5,r0
	orr r5,r5,#0x8000

	strh r5,[r6,r4]
	subs r4,r4,#2
	bpl noMap

	ldmfd sp!,{r4-r8,lr}
	bx lr

;@----------------------------------------------------------------------------
gPrefix:
	orr r0,r0,r0,lsl#4
	mov r2,r8
;@----------------------------------------------------------------------------
contrastConvert:	;@ Takes value in r0(0-0xFF), gamma in r1(0-4), contrast in r2(0-255) returns new value in r0=0x1F
;@----------------------------------------------------------------------------
	rsb r3,r2,#256
	mul r0,r3,r0
	add r0,r0,r2,lsl#7
	mov r0,r0,lsr#8
;@----------------------------------------------------------------------------
gammaConvert:	;@ Takes value in r0(0-0xFF), gamma in r1(0-4),returns new value in r0=0x1F
;@----------------------------------------------------------------------------
	rsb r2,r0,#0x100
	mul r3,r2,r2
	rsbs r2,r3,#0x10000
	rsb r3,r1,#4
	orr r0,r0,r0,lsl#8
	mul r2,r1,r2
	mla r0,r3,r0,r2
	movs r0,r0,lsr#13

	bx lr

;@----------------------------------------------------------------------------
paletteTxAll:				;@ Called from ui.c
	.type paletteTxAll STT_FUNC
;@----------------------------------------------------------------------------
	ldr r0,=EMUPALBUFF
	ldr mikptr,=mikey_0
;@----------------------------------------------------------------------------
paletteTx:					;@ r0=destination, mikptr=Mikey
;@----------------------------------------------------------------------------
	ldr r1,=MAPPED_RGB
	ldr r2,=0x1FFE
	stmfd sp!,{r4-r8,lr}
	mov r5,#0

	add r4,mikptr,#mikPalette
txLoop:
	ldr r3,[r4],#4
	and r3,r2,r3,lsl#1
	ldrh r3,[r1,r3]
	strh r3,[r0,r5]			;@ Background palette

	add r5,r5,#2
	cmp r5,#0x20
	bmi txLoop

	ldmfd sp!,{r4-r8,lr}
	bx lr

;@----------------------------------------------------------------------------
lodjurRenderCallback:		;@ (u8 *ram, u32 *palette, bool flip, bool palChg)
;@----------------------------------------------------------------------------
	stmfd sp!,{r4-r6,lr}
	ldr r5,=PAL_CACHE
	cmp r3,#0
	beq palCacheOk
	ldr r4,=MAPPED_RGB
	mov r6,#16
palCacheLoop:
	subs r6,r6,#1
	ldr r3,[r1,r6,lsl#2]
	mov r3,r3,lsl#1
	ldrh r3,[r4,r3]
	str r3,[r5,r6,lsl#2]
	bne palCacheLoop

palCacheOk:
	ldr r1,currentDest

	mov r4,#GAME_WIDTH/2
	cmp r2,#0
	bne rendLoopFlip
rendLoop:
	ldrb r2,[r0],#1
	and r3,r2,#0x0F
	and r2,r2,#0xF0
	ldr r3,[r5,r3,lsl#2]
	ldr r2,[r5,r2,lsr#2]
	orr r3,r2,r3,lsl#16
	str r3,[r1],#4
	subs r4,r4,#1
	bne rendLoop
	add r1,r1,#(SCREEN_WIDTH-GAME_WIDTH)*2
	str r1,currentDest
	ldmfd sp!,{r4-r6,pc}
rendLoopFlip:
	ldrb r2,[r0],#-1
	and r3,r2,#0x0F
	and r2,r2,#0xF0
	ldr r3,[r5,r3,lsl#2]
	ldr r2,[r5,r2,lsr#2]
	orr r3,r3,r2,lsl#16
	str r3,[r1],#4
	subs r4,r4,#1
	bne rendLoopFlip
	add r1,r1,#(SCREEN_WIDTH-GAME_WIDTH)*2
	str r1,currentDest
	ldmfd sp!,{r4-r6,pc}

;@----------------------------------------------------------------------------
setScreenRefresh:			;@ r0 in = Lynx cycles per frame.
;@----------------------------------------------------------------------------
	bx lr

;@----------------------------------------------------------------------------
//#ifdef GBA
//	.section .iwram, "ax", %progbits	;@ For the GBA
//#endif
;@----------------------------------------------------------------------------
vblIrqHandler:
	.type vblIrqHandler STT_FUNC
;@----------------------------------------------------------------------------
	stmfd sp!,{r4-r8,lr}
	bl vblSound1
	bl calculateFPS

	mov r6,#REG_BASE
	strh r6,[r6,#REG_DMA0CNT_H]	;@ DMA0 stop
	strh r6,[r6,#REG_DMA3CNT_H]	;@ DMA3 stop

	add r0,r6,#REG_DMA0SAD
	ldr r1,dmaScroll			;@ Setup DMA buffer for scrolling:
	ldmia r1!,{r3-r4}			;@ Read
	add r2,r6,#REG_BG0HOFS		;@ DMA0 always goes here
	stmia r2,{r3-r4}			;@ Set 1st values manually, HBL is AFTER 1st line
	ldr r3,=0xA6600002			;@ hblank 32bit repeat incsrc inc_reloaddst, 2 word
//	stmia r0,{r1-r3}			;@ DMA0 go

	ldr r1,=EMUPALBUFF			;@ DMA3 src, Palette transfer:
	mov r2,#BG_PALETTE			;@ DMA3 dst
	mov r3,#0x84000000			;@ 32bit incsrc incdst
	orr r3,r3,#0x100			;@ 256 words (1024 bytes)
	stmia r0,{r1-r3}			;@ DMA3 go

	ldr r0,=GFX_DISPCNT
	ldr r0,[r0]
	ldrb r2,gGfxMask
	bic r0,r0,r2,lsl#8
	strh r0,[r6,#REG_DISPCNT]

	ldrb r0,frameDone
	cmp r0,#0
	beq nothingNew
	mov r0,#0
	strb r0,frameDone
nothingNew:

	bl scanKeys
	bl vblSound2
	ldmfd sp!,{r4-r8,lr}
	bx lr

;@----------------------------------------------------------------------------
gfxRefresh:					;@ Called from C when changing scaling.
	.type gfxRefresh STT_FUNC
;@----------------------------------------------------------------------------
;@----------------------------------------------------------------------------
gfxEndFrame:				;@ Called just after screen end (line 144)	(r0-r3 safe to use)
;@----------------------------------------------------------------------------
	stmfd sp!,{r4-r8,lr}

	ldr r0,=(((SCREEN_HEIGHT-GAME_HEIGHT)/2) * SCREEN_WIDTH * 2)
	add r0,r0,#SCREEN_WIDTH-GAME_WIDTH
	add r0,r0,#0x06000000
	str r0,currentDest

	bl paletteTxAll
;@--------------------------

	mov r0,#1
	strb r0,frameDone

	ldr r1,=fpsValue
	ldr r0,[r1]
	add r0,r0,#1
	str r0,[r1]

	ldr r1,frameTotal
	add r1,r1,#1
	str r1,frameTotal

	ldmfd sp!,{r4-r8,lr}
	bx lr

;@----------------------------------------------------------------------------
frameTotal:		.long 0			;@ Let Gui.c see frame count for savestates

tmpScroll:		.long SCROLLBUFF1
dmaScroll:		.long SCROLLBUFF2

gFlicker:		.byte 1
				.space 2
gTwitch:		.byte 0

gGfxMask:		.byte 0
frameDone:		.byte 0
				.byte 0,0
;@----------------------------------------------------------------------------
suzyReset0:		;@ r0=ram+LUTs
;@----------------------------------------------------------------------------
	ldr suzptr,=suzy_0
	b suzyReset
;@----------------------------------------------------------------------------
lnxSuzySetButtonData:
	.type lnxSuzySetButtonData STT_FUNC
;@----------------------------------------------------------------------------
	ldr suzptr,=suzy_0
	b suzySetButtonData
;@----------------------------------------------------------------------------
	.section .ewram, "ax"

gfxState:
currentDest:
	.long 0
	.long 0
	.long 0,0,0
lcdSkip:
	.long 0

GFX_DISPCNT:
	.long 0
GFX_BG0CNT:
	.short 0
GFX_BG1CNT:
	.short 0

#ifdef GBA
	.section .sbss				;@ This is EWRAM on GBA with devkitARM
#else
	.section .bss
#endif
	.align 2
SCROLLBUFF1:
	.space SCREEN_HEIGHT*8		;@ Scrollbuffer.
SCROLLBUFF2:
	.space SCREEN_HEIGHT*8		;@ Scrollbuffer.
MAPPED_RGB:
	.space 0x2000				;@ 4096*2
MAPPED_BNW:
	.space 0x20
EMUPALBUFF:
	.space 0x400
PAL_CACHE:
	.space 0x40					;@ 16*4

#ifdef GBA
	.section .bss				;@ This is IWRAM on GBA with devkitARM
#endif
;@----------------------------------------------------------------------------
suzy_0:
	.space suzySize
;@----------------------------------------------------------------------------
	.end
#endif // #ifdef __arm__
