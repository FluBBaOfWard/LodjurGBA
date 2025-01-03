# LodjurGBA V0.1.0

<img align="right" width="220" src="./logo.png" alt="The Lynx logo"/>

An Atari Lynx emulator for Nintendo GBA.

## How to use

When the emulator starts, you press L+R to open up the menu.
Now you can use the cross to navigate the menus, A to select an option,
B to go back a step.

## Menu

### File

* Load Game: Select a game to load.
* Load NVRAM: Load non volatile ram (EEPROM/SRAM) for the currently running game.
* Save NVRAM: Save non volatile ram (EEPROM/SRAM) for the currently running game.
* Save Settings: Save the current settings (and internal EEPROM).
* Reset Game: Reset the currently running game.

### Controller

* Autofire: Select if you want autofire.
* Swap A-B: Swap which GBA button is mapped to which Lynx button.

### Display

* Gamma: Lets you change the gamma ("brightness").
* Contrast: Lets you change the contrast.

### Settings

* Speed: Switch between speed modes.
  * Normal: Game runs at it's normal speed.
  * 200%: Game runs at double speed.
  * Max: Games can run up to 4 times normal speed (might change).
  * 50%: Game runs at half speed.
* Autoload State: Toggle Savestate autoloading. Automagically load the savestate associated with the selected game.
* Autoload NVRAM: Toggle EEPROM/SRAM autoloading. Automagically load the EEPROM/SRAM associated with the selected game.
* Autosave Settings: This will save settings when leaving menu if any changes are made.
* Autopause Game: Toggle if the game should pause when opening the menu.
* Overclock EWRAM: Changes the waitstates on EWRAM between 2 and 1, might damage your GBA and uses more power, around 10% speedgain. Use at your own risk!
* Autosleep: Change the autosleep time, also see Sleep.

### Machine

* Machine: Select the emulated machine.
* Cpu speed hacks: Allow speed hacks.
* Half Cpu Speed: Just half the emulated cpu speed.
* Sound: Turn sound emulation on/off.

### Debug

* Debug Output: Show FPS and logged text.
* Step Frame: Emulate one frame.

### About

Some info about the emulator and game...

### Sleep

Put the GBA into sleepmode. START+SELECT wakes up from sleep mode (activated
 from this menu or from 5/10/30 minutes of inactivity).

## Controls

* GBA B & A buttons are mapped to Lynx B & A.
* GBA d-pad is mapped to Lynx d-pad.
* GBA Start is mapped to Lynx Pause.

## Games

* Baseball Heroes. Doesn't start'
* Gates of zendocon. Hangs if intro is not skipped.

## Credits

```text
Thanks to:
Keith Wilkins for Handy source and info.
```

Fredrik Ahlström

<https://bsky.app/profile/therealflubba.bsky.social>
<https://www.github.com/FluBBaOfWard>
X/Twitter @TheRealFluBBa
