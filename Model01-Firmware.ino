// -*- mode: c++ -*-
// Copyright 2016 Keyboardio, inc. <jesse@keyboard.io>
// Copyright 2017 Iliana Weller <ilianaw@buttslol.net>
// See "LICENSE" for license details

#ifndef BUILD_INFORMATION
#define BUILD_INFORMATION "locally built"
#endif

#include "Kaleidoscope.h"
#include "Kaleidoscope-Macros.h"
#include "Kaleidoscope-LEDControl.h"
#include "LED-Off.h"
#include "Kaleidoscope-LEDEffect-SolidColor.h"
#include "Kaleidoscope-Focus.h"
#include "Kaleidoscope-HostPowerManagement.h"


#define isModifier(key) (key.raw >= Key_LeftControl.raw && key.raw <= Key_RightGui.raw)

enum { MACRO_ANY,
       MACRO_WOBIPV4,
       MACRO_WOBIPV6,
     }; // macros

enum { QWERTY, FUNCTION, TMUX }; // layers

// *INDENT-OFF*

const Key keymaps[][ROWS][COLS] PROGMEM = {

  [QWERTY] = KEYMAP_STACKED
  (___,          Key_1, Key_2, Key_3, Key_4, Key_5, Key_LEDEffectNext,
   Key_Backtick, Key_Q, Key_W, Key_E, Key_R, Key_T, Key_Tab,
   Key_PageUp,   Key_A, Key_S, Key_D, Key_F, Key_G,
   Key_PageDown, Key_Z, Key_X, Key_C, Key_V, Key_B, Key_Escape,
   Key_LeftControl, Key_Backspace, Key_LeftGui, Key_LeftShift,
   ShiftToLayer(FUNCTION),

   M(MACRO_ANY),       Key_6, Key_7, Key_8,     Key_9,         Key_0,         ___,
   Key_Enter,          Key_Y, Key_U, Key_I,     Key_O,         Key_P,         Key_Equals,
                       Key_H, Key_J, Key_K,     Key_L,         Key_Semicolon, Key_Quote,
   ShiftToLayer(TMUX), Key_N, Key_M, Key_Comma, Key_Period,    Key_Slash,     Key_Minus,
   Key_RightShift,     Key_LeftAlt, Key_Spacebar, Key_RightControl,
   ShiftToLayer(FUNCTION)),

  [FUNCTION] = KEYMAP_STACKED
  (___,      Key_F1,          Key_F2,     Key_F3, Key_F4, Key_F5, XXX,
   Key_Tab,  ___,             ___,        ___,    ___,    ___,    ___,
   Key_Home, ___,             ___,        ___,    ___,    ___,
   Key_End,  Key_PrintScreen, Key_Insert, ___,    M(MACRO_WOBIPV6), M(MACRO_WOBIPV4), ___,
   ___, Key_Delete, ___, ___,
   ___,

   Consumer_ScanPreviousTrack, Key_F6,                 Key_F7,                   Key_F8,                   Key_F9,          Key_F10,          Key_F11,
   Consumer_PlaySlashPause,    Consumer_ScanNextTrack, Key_LeftCurlyBracket,     Key_RightCurlyBracket,    Key_LeftBracket, Key_RightBracket, Key_F12,
                               Key_LeftArrow,          Key_DownArrow,            Key_UpArrow,              Key_RightArrow,  ___,              ___,
   Key_PcApplication,          Key_Mute,               Consumer_VolumeDecrement, Consumer_VolumeIncrement, ___,             Key_Backslash,    Key_Pipe,
   ___, Key_RightAlt, Key_Enter, ___,
   ___),

  [TMUX] = KEYMAP_STACKED
  (___, ___, ___, ___, ___, ___, ___,
   ___, ___, ___, ___, ___, ___, ___,
   ___, ___, ___, XXX, ___, ___,
   ___, ___, ___, ___, ___, LCTRL(Key_B), ___,
   ___, ___, ___, ___,
   XXX,

   ___, ___, ___, ___, ___, ___, ___,
   ___, ___, ___, ___, ___, ___, ___,
        Key_LeftArrow, Key_DownArrow, Key_UpArrow, Key_RightArrow, ___, ___,
   ___, ___, ___, ___, ___, ___, ___,
   ___, ___, ___, ___,
   XXX),

};

// *INDENT-ON*

static void anyKeyMacro(uint8_t keyState) {
  static Key lastKey;
  if (keyToggledOn(keyState))
    lastKey.keyCode = Key_A.keyCode + (uint8_t)(millis() % 36);

  if (keyIsPressed(keyState))
    kaleidoscope::hid::pressKey(lastKey);
}

static void wobscaleIPv4Macro(uint8_t keyState) {
  if (keyToggledOn(keyState))
    Macros.type(PSTR("209.251.245."));
}

static void wobscaleIPv6Macro(uint8_t keyState) {
  if (keyToggledOn(keyState))
    Macros.type(PSTR("2620:fc:c000::"));
}

const macro_t *macroAction(uint8_t macroIndex, uint8_t keyState) {
  switch (macroIndex) {
  case MACRO_ANY:
    anyKeyMacro(keyState);
    break;
  case MACRO_WOBIPV4:
    wobscaleIPv4Macro(keyState);
    break;
  case MACRO_WOBIPV6:
    wobscaleIPv6Macro(keyState);
    break;
  }
  return MACRO_NONE;
}

Key tmuxEventHandlerHook(Key key, byte row, byte col, uint8_t keyState) {
  if (keyState & INJECTED || isModifier(key))
    return key;

  if (keyToggledOn(keyState) && key != ShiftToLayer(TMUX) && Layer.isOn(TMUX)) {
    handleKeyswitchEvent(LCTRL(Key_B), UNKNOWN_KEYSWITCH_LOCATION, IS_PRESSED | INJECTED);
    kaleidoscope::hid::sendKeyboardReport();
    handleKeyswitchEvent(LCTRL(Key_B), UNKNOWN_KEYSWITCH_LOCATION, WAS_PRESSED | INJECTED);
    kaleidoscope::hid::sendKeyboardReport();
  }

  return key;
}

// These 'solid' color effect definitions define a rainbow of
// LED color modes calibrated to draw 500mA or less on the
// Keyboardio Model 01.
static kaleidoscope::LEDSolidColor solidViolet(130, 0, 120);

void toggleLedsOnSuspendResume(kaleidoscope::HostPowerManagement::Event event) {
  switch (event) {
  case kaleidoscope::HostPowerManagement::Suspend:
    LEDControl.paused = true;
    LEDControl.set_all_leds_to({0, 0, 0});
    LEDControl.syncLeds();
    break;
  case kaleidoscope::HostPowerManagement::Resume:
    LEDControl.paused = false;
    LEDControl.refreshAll();
    break;
  case kaleidoscope::HostPowerManagement::Sleep:
    break;
  }
}

void hostPowerManagementEventHandler(kaleidoscope::HostPowerManagement::Event event) {
  toggleLedsOnSuspendResume(event);
}

void setup() {
  Serial.begin(9600);
  Kaleidoscope.setup();
  Kaleidoscope.use(
    &LEDControl,
    &LEDOff,
    &solidViolet,
    &Macros,
    &Focus,
    &HostPowerManagement
  );

  LEDOff.activate();

  Focus.addHook(FOCUS_HOOK_HELP);
  Focus.addHook(FOCUS_HOOK_VERSION);

  Kaleidoscope.useEventHandlerHook(tmuxEventHandlerHook);
  HostPowerManagement.enableWakeup();
}

void loop() {
  Kaleidoscope.loop();
}
