// vim: set ft=cpp:
// Copyright 2016 Keyboardio, inc. <jesse@keyboard.io>
// Copyright 2019 iliana weller <ilianaw@buttslol.net>
// See "LICENSE" for license details

#include "Kaleidoscope.h"
#include "Kaleidoscope-HostOS.h"
#include "Kaleidoscope-Macros.h"
#include "Kaleidoscope-LEDControl.h"
#include "Kaleidoscope-LEDEffect-SolidColor.h"
#include "Kaleidoscope-LEDEffect-DigitalRain.h"
#include "Kaleidoscope-Unicode.h"
#include "Kaleidoscope-Syster.h"
#include "Kaleidoscope-HostPowerManagement.h"
#include "Kaleidoscope-USB-Quirks.h"
#include "Kaleidoscope-PrefixLayer.h"


enum { MACRO_WOBIPV4,
       MACRO_WOBIPV6,
       MACRO_ZWS,
       MACRO_IMDS,
     }; // macros

enum { QWERTY, FUNCTION, TMUX }; // layers

static const kaleidoscope::plugin::PrefixLayer::dict_t prefixlayerdict[] PROGMEM = PREFIX_DICT({TMUX, PREFIX_SEQ(LCTRL(Key_B))});

// *INDENT-OFF*

KEYMAPS(

  [QWERTY] = KEYMAP_STACKED
  (___,          Key_1, Key_2, Key_3, Key_4, Key_5, Key_LEDEffectNext,
   Key_Backtick, Key_Q, Key_W, Key_E, Key_R, Key_T, Key_Tab,
   Key_PageUp,   Key_A, Key_S, Key_D, Key_F, Key_G,
   Key_PageDown, Key_Z, Key_X, Key_C, Key_V, Key_B, Key_Escape,
   Key_LeftControl, Key_Backspace, Key_LeftGui, Key_LeftShift,
   ShiftToLayer(FUNCTION),

   SYSTER,             Key_6, Key_7, Key_8,     Key_9,         Key_0,         LSHIFT(LALT(Key_DownArrow)),
   Key_Enter,          Key_Y, Key_U, Key_I,     Key_O,         Key_P,         Key_Equals,
                       Key_H, Key_J, Key_K,     Key_L,         Key_Semicolon, Key_Quote,
   ShiftToLayer(TMUX), Key_N, Key_M, Key_Comma, Key_Period,    Key_Slash,     Key_Minus,
   Key_RightShift,     Key_LeftAlt, Key_Spacebar, Key_RightControl,
   ShiftToLayer(FUNCTION)),

  [FUNCTION] = KEYMAP_STACKED
  (___,      Key_F1,       Key_F2, Key_F3, Key_F4,           Key_F5,           Key_LEDEffectPrevious,
   Key_Tab,  ___,          ___,    ___,    ___,              ___,              ___,
   Key_Home, ___,          ___,    ___,    ___,              ___,
   Key_End,  M(MACRO_ZWS), ___,    ___,    M(MACRO_WOBIPV6), M(MACRO_WOBIPV4), ___,
   ___, Key_Delete, ___, ___,
   ___,

   Consumer_ScanPreviousTrack, Key_F6,                 Key_F7,                   Key_F8,                   Key_F9,          Key_F10,          Key_F11,
   Consumer_PlaySlashPause,    Consumer_ScanNextTrack, Key_LeftCurlyBracket,     Key_RightCurlyBracket,    Key_LeftBracket, Key_RightBracket, Key_F12,
                               Key_LeftArrow,          Key_DownArrow,            Key_UpArrow,              Key_RightArrow,  ___,              ___,
   Key_PcApplication,          Consumer_Mute,          Consumer_VolumeDecrement, Consumer_VolumeIncrement, M(MACRO_IMDS),   Key_Backslash,    Key_Pipe,
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
   ___, ___, Key_LeftCurlyBracket, Key_RightCurlyBracket, Key_LeftBracket, Key_RightBracket, ___,
        Key_LeftArrow, Key_DownArrow, Key_UpArrow, Key_RightArrow, ___, ___,
   ___, ___, ___, ___, ___, ___, ___,
   ___, ___, ___, ___,
   XXX),

)

// *INDENT-ON*

const macro_t *macroAction(uint8_t macroIndex, uint8_t keyState) {
  switch (macroIndex) {
  case MACRO_WOBIPV4:
    if (keyToggledOn(keyState))
      Macros.type(PSTR("209.251.245."));
    break;
  case MACRO_WOBIPV6:
    if (keyToggledOn(keyState))
      Macros.type(PSTR("2620:fc:c000::"));
    break;
  case MACRO_ZWS:
    if (keyToggledOn(keyState))
      Unicode.type(0x200b);
    break;
  case MACRO_IMDS:
    if (keyToggledOn(keyState))
      Macros.type(PSTR("169.254.169.254/latest"));
    break;
  }
  return MACRO_NONE;
}

void systerAction(kaleidoscope::plugin::Syster::action_t action, const char *symbol) {
  switch (action) {
  case kaleidoscope::plugin::Syster::StartAction:
    break;
  case kaleidoscope::plugin::Syster::EndAction:
    break;
  case kaleidoscope::plugin::Syster::SymbolAction:
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
static kaleidoscope::plugin::LEDSolidColor solidViolet(130, 0, 120);

void toggleLedsOnSuspendResume(kaleidoscope::plugin::HostPowerManagement::Event event) {
  switch (event) {
  case kaleidoscope::plugin::HostPowerManagement::Suspend:
    LEDControl.set_all_leds_to({0, 0, 0});
    LEDControl.syncLeds();
    LEDControl.paused = true;
    break;
  case kaleidoscope::plugin::HostPowerManagement::Resume:
    LEDControl.paused = false;
    LEDControl.refreshAll();
    break;
  case kaleidoscope::plugin::HostPowerManagement::Sleep:
    break;
  }
}

void hostPowerManagementEventHandler(kaleidoscope::plugin::HostPowerManagement::Event event) {
  toggleLedsOnSuspendResume(event);
}

class SysterLED_ : public kaleidoscope::Plugin {
 public:
  SysterLED_() {}
  kaleidoscope::EventHandlerResult beforeEachCycle() {
    if (Syster.is_active())
      LEDControl.setCrgbAt(0, 0, {0xff, 0xff, 0xff});
    else
      LEDControl.refreshAt(0, 0);
    return kaleidoscope::EventHandlerResult::OK;
  }
};
SysterLED_ SysterLED;

KALEIDOSCOPE_INIT_PLUGINS(
  Syster,
  LEDControl,
  LEDDigitalRainEffect,
  solidViolet,
  LEDOff,
  HostOS,
  Macros,
  Unicode,
  HostPowerManagement,
  PrefixLayer,
  SysterLED
);

void setup() {
  Serial.begin(9600);
  Kaleidoscope.setup();

  HostOS.os(kaleidoscope::hostos::LINUX);

  PrefixLayer.dict = prefixlayerdict;
}

void loop() {
  Kaleidoscope.loop();
}
