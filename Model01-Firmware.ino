// -*- mode: c++ -*-
// Copyright 2016 Keyboardio, inc. <jesse@keyboard.io>
// Copyright 2018 Iliana Weller <ilianaw@buttslol.net>
// See "LICENSE" for license details

#include "Kaleidoscope.h"
#include "Kaleidoscope-HostOS.h"
#include "Kaleidoscope/HostOS-select.h"
#include "Kaleidoscope-Macros.h"
#include "Kaleidoscope-LEDControl.h"
#include "LED-Off.h"
#include "Kaleidoscope-LEDEffect-SolidColor.h"
#include "Kaleidoscope-LEDEffect-DigitalRain.h"
#include "Kaleidoscope-Focus.h"
#include "Kaleidoscope-Unicode.h"
#include "Kaleidoscope-Syster.h"
#include "Kaleidoscope-HostPowerManagement.h"
#include "Kaleidoscope-PrefixLayer.h"


enum { MACRO_ANY,
       MACRO_WOBIPV4,
       MACRO_WOBIPV6,
       MACRO_HAMMER_AND_SICKLE,
       MACRO_TRANS,
       MACRO_RUST_DEBUG,
       MACRO_ZWJ,
     }; // macros

enum { QWERTY, FUNCTION, TMUX }; // layers

static const kaleidoscope::PrefixLayer::dict_t prefixlayerdict[] PROGMEM = PREFIX_DICT({TMUX, PREFIX_SEQ(LCTRL(Key_B))});

// *INDENT-OFF*

KEYMAPS(

  [QWERTY] = KEYMAP_STACKED
  (SYSTER,       Key_1, Key_2, Key_3, Key_4, Key_5, Key_LEDEffectNext,
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
  (___,      Key_F1,          Key_F2,     Key_F3,                     Key_F4,           Key_F5,           Key_LEDEffectPrevious,
   Key_Tab,  ___,             ___,        ___,                        ___,              M(MACRO_TRANS),   ___,
   Key_Home, ___,             ___,        M(MACRO_RUST_DEBUG),        ___,              ___,
   Key_End,  M(MACRO_ZWJ),    ___,        M(MACRO_HAMMER_AND_SICKLE), M(MACRO_WOBIPV6), M(MACRO_WOBIPV4), ___,
   ___, Key_Delete, ___, ___,
   ___,

   Consumer_ScanPreviousTrack, Key_F6,                 Key_F7,                   Key_F8,                   Key_F9,          Key_F10,          Key_F11,
   Consumer_PlaySlashPause,    Consumer_ScanNextTrack, Key_LeftCurlyBracket,     Key_RightCurlyBracket,    Key_LeftBracket, Key_RightBracket, Key_F12,
                               Key_LeftArrow,          Key_DownArrow,            Key_UpArrow,              Key_RightArrow,  ___,              ___,
   Key_PcApplication,          Consumer_Mute,          Consumer_VolumeDecrement, Consumer_VolumeIncrement, ___,             Key_Backslash,    Key_Pipe,
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
   ___, ___, Key_LeftCurlyBracket, Key_RightCurlyBracket, Key_LeftBracket, ___, ___,
        Key_LeftArrow, Key_DownArrow, Key_UpArrow, Key_RightArrow, ___, ___,
   ___, ___, ___, ___, ___, ___, ___,
   ___, ___, ___, ___,
   XXX),

)

// *INDENT-ON*

static void anyKeyMacro(uint8_t keyState) {
  static Key lastKey;
  if (keyToggledOn(keyState))
    lastKey.keyCode = Key_A.keyCode + (uint8_t)(millis() % 36);

  if (keyIsPressed(keyState))
    kaleidoscope::hid::pressKey(lastKey);
}

const macro_t *macroAction(uint8_t macroIndex, uint8_t keyState) {
  switch (macroIndex) {
  case MACRO_ANY:
    anyKeyMacro(keyState);
    break;
  case MACRO_WOBIPV4:
    if (keyToggledOn(keyState))
      Macros.type(PSTR("209.251.245."));
    break;
  case MACRO_WOBIPV6:
    if (keyToggledOn(keyState))
      Macros.type(PSTR("2620:fc:c000::"));
    break;
  case MACRO_HAMMER_AND_SICKLE:
    if (keyToggledOn(keyState))
      Unicode.type(0x262d);
    break;
  case MACRO_TRANS:
    if (keyToggledOn(keyState))
      Unicode.type(0x26a7);
    break;
  case MACRO_RUST_DEBUG:
    // println!("{:?}", ); + left arrow x 2
    return MACRODOWN(T(P), T(R), T(I), T(N), T(T), T(L), T(N),
                     D(LeftShift), T(1), T(9), T(Quote), U(LeftShift),
                     T(LeftCurlyBracket),
                     D(LeftShift), T(Semicolon), T(Slash), U(LeftShift),
                     T(RightCurlyBracket),
                     D(LeftShift), T(Quote), U(LeftShift),
                     T(Comma), T(Spacebar),
                     D(LeftShift), T(0), U(LeftShift),
                     T(Semicolon), T(LeftArrow), T(LeftArrow));
    break;
  case MACRO_ZWJ:
    if (keyToggledOn(keyState))
      Unicode.type(0x200d);
    break;
  }
  return MACRO_NONE;
}

void systerAction(kaleidoscope::Syster::action_t action, const char *symbol) {
  switch (action) {
  case kaleidoscope::Syster::StartAction:
    break;
  case kaleidoscope::Syster::EndAction:
    break;
  case kaleidoscope::Syster::SymbolAction:
    if (strcmp(symbol, "degree") == 0)
      Unicode.type(0x00b0);
    else if (strcmp(symbol, "em") == 0)
      Unicode.type(0x2014);
    break;
  }
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

void loopHook(bool postClear) {
  if (postClear)
    return;

  if (Syster.is_active())
    LEDControl.setCrgbAt(0, 0, {0xff, 0xff, 0xff});
  else
    LEDControl.refreshAt(0, 0);
}

void setup() {
  Serial.begin(9600);
  Kaleidoscope.setup();
  Kaleidoscope.use(
    &Syster,
    &LEDControl,
    &LEDDigitalRainEffect,
    &solidViolet,
    &LEDOff,
    &HostOS,
    &Macros,
    &Focus,
    &Unicode,
    &HostPowerManagement,
    &PrefixLayer
  );

  /*
   * FIXME: Work around for https://github.com/keyboardio/Kaleidoscope-Syster/issues/3
   * https://trunk.mad-scientist.club/@algernon/99518558666830872
   */
  KeyboardHardware.maskKey(0, 0);

  HostOS.os(kaleidoscope::hostos::LINUX);

  Focus.addHook(FOCUS_HOOK_HOSTOS);
  Focus.addHook(FOCUS_HOOK_HELP);
  Focus.addHook(FOCUS_HOOK_VERSION);

  HostPowerManagement.enableWakeup();

  PrefixLayer.dict = prefixlayerdict;

  Kaleidoscope.useLoopHook(loopHook);
}

void loop() {
  Kaleidoscope.loop();
}
