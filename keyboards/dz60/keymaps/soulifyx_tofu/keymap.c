#include QMK_KEYBOARD_H
#include "dz60.h"
#include "led.h"

enum keyboard_layers {
  _BASE = 0, // Base Layer 
  _FKEYS,      // Control Layer
  _CALC,      // Function Layer
  RGB_LYR
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[_BASE] = LAYOUT_60_b_ansi(KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_BSPC, KC_BSPC, 
    KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC, KC_BSLS, 
    KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_ENT, 
    KC_LSPO, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_RSPC, KC_UP, KC_SLSH, 
    KC_LCTL, KC_LGUI, KC_LALT, KC_SPC, KC_SPC, KC_SPC, TG(1), TG(2), KC_LEFT, KC_DOWN, KC_RGHT),
  [_FKEYS] = LAYOUT_60_b_ansi(KC_GRV, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, KC_DEL, KC_DEL, 
    KC_NO, RGB_TOG, RGB_MOD, RGB_RMOD, RGB_HUI, RGB_HUD, RGB_SAI, RGB_SAD, RGB_VAI, RGB_VAD, RGB_SPI, RGB_SPD, KC_NO, KC_PSCR, 
    KC_NO, RGB_M_P, RGB_M_B, RGB_M_R, RGB_M_SW, RGB_M_SN, RGB_M_K, RGB_M_X, RGB_M_G, KC_NO, KC_NO, KC_NO, KC_ENT, 
    KC_NO, BL_TOGG, BL_STEP, BL_ON, BL_OFF, BL_INC, BL_DEC, BL_BRTG, KC_NO, KC_NO, KC_NO, KC_VOLU, KC_MUTE, 
    KC_NO, KC_LGUI, RGB_LYR, KC_SPC, KC_SPC, KC_SPC, KC_TRNS, KC_TRNS, KC_MPRV, KC_VOLD, KC_MNXT),
	[_CALC] = LAYOUT_60_b_ansi(KC_NO, KC_PSLS, KC_PAST, KC_PMNS, KC_PPLS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_BSPC, KC_BSPC, 
    KC_NO, KC_P7, KC_P8, KC_P9, KC_PCMM, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, 
    KC_NO, KC_P4, KC_P5, KC_P6, KC_PDOT, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_PENT, 
    KC_NO, KC_P1, KC_P2, KC_P3, KC_PEQL, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_UP, KC_NO,
    KC_NO, KC_NO, KC_NO, KC_P0, KC_P0, KC_P0, KC_NO, KC_TRNS, KC_LEFT, KC_DOWN, KC_RGHT)
	
};

typedef union {
  uint32_t raw;
  struct {
    bool     rgb_layer_change :1;
  };
} user_config_t;

user_config_t user_config;

void matrix_init_kb(void) {
  // Keyboard start-up code goes here
  // Runs once when the firmware starts up
  user_config.raw = eeconfig_read_user();

  if(user_config.rgb_layer_change) {
    rgblight_enable_noeeprom();
    rgblight_sethsv_noeeprom_turquoise(); 
    rgblight_mode_noeeprom(1);
  }

  matrix_init_user();
  led_init_ports();
};

void matrix_scan_kb(void) {
  // Looping keyboard code goes here
  // This runs every cycle (a lot)
  matrix_scan_user();
};

void led_init_ports(void) {
  // Set caps lock LED pin as output
  DDRB |= (1 << 2);
  // Default to off
  PORTB |= (1 << 2);
}

void led_set_kb(uint8_t usb_led) {
    if (usb_led & (1 << USB_LED_CAPS_LOCK)) {
        PORTB &= ~(1 << 2);
    } else {
        PORTB |= (1 << 2);
    }

    led_set_user(usb_led);
}

/*void suspend_power_down_user(void) // When idle suspend RGB underglow
{
    rgb_matrix_set_suspend_state(true);
}

void suspend_wakeup_init_user(void) // Turns on RGB underglow if user wakes up
{
    rgb_matrix_set_suspend_state(false);
}*/

uint32_t layer_state_set_user(uint32_t state) { // Runs everytime changing layer
    switch (biton32(state)) {
    case _CALC:
        if (user_config.rgb_layer_change) { rgblight_mode_noeeprom(4); rgblight_sethsv_noeeprom_coral();  } //4
        break;
    case _FKEYS:
        if (user_config.rgb_layer_change) { rgblight_mode_noeeprom(14); rgblight_sethsv_noeeprom(0,210,120); } //14
        break;
    default: //  for any other layers, or the default layer
        if (user_config.rgb_layer_change) { rgblight_mode_noeeprom(1); rgblight_sethsv_noeeprom_turquoise(); }
        break;
    }
  return state;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) { // Adds another event on top of the original function of the pressed key
  // Returns true if the 66666666666655original function still wants to be run
  // Returns false to only run the added event and cancel the original function of the key

  switch (keycode) {
    case RGB_LYR:  // This allows me to use underglow as layer indication, or as normal
        if (record->event.pressed) { 
            user_config.rgb_layer_change ^= 1; // Toggles the status
            eeconfig_update_user(user_config.raw); // Writes the new status to EEPROM
            if (user_config.rgb_layer_change) { // if layer state indication is enabled, 
                layer_state_set(layer_state);   // then immediately update the layer color
            }
        }
        return false; break;
    case RGB_MODE_FORWARD ... RGB_MODE_GRADIENT: // For any of the RGB codes (see quantum_keycodes.h, L400 for reference)
        if (record->event.pressed) { //This disables layer indication, as it's assumed that if you're changing this ... you want that disabled
            if (user_config.rgb_layer_change) {        // only if this is enabled 
                user_config.rgb_layer_change = false;  // disable it, and 
                eeconfig_update_user(user_config.raw); // write the setings to EEPROM
            }
        }
        return true; break;
    default:
      return true; // Process all other keycodes normally
  }
}

void eeconfig_init_user(void) {  // EEPROM is getting reset! 
  user_config.rgb_layer_change = true; // We want this enabled by default
  eeconfig_update_user(user_config.raw); // Write default value to EEPROM now

  // use the non noeeprom versions, to write these values to EEPROM too
  rgblight_enable(); // Enable RGB by default
  rgblight_sethsv_turquoise();  // Set it to CYAN by default
  rgblight_mode(1); // set to solid by default
}