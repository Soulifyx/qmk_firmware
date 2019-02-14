#include QMK_KEYBOARD_H
#include "dz60.h"
#include "led.h"

enum keyboard_layers {
  _BASE = 0, // Base Layer 
  _FKEYS,      // Control Layer
  _CALC,      // Function Layer
  RGB_LYR
};

enum tap_dance{
  SINGLE_TAP = 1,
  SINGLE_HOLD,
  DOUBLE_TAP,
  DOUBLE_HOLD,
  TRIPLE_TAP,
  TRIPLE_HOLD
};

//Tap dance enums
enum {
  ALT_OSRAISE = 0
};

enum combos {
  BSP_PENT
};

typedef union {
  uint32_t raw;
  struct {
    bool     rgb_layer_change :1;
  };
} user_config_t;

user_config_t user_config;

static int alttap_state = 0;

// Backlight timeout feature
#define BACKLIGHT_TIMEOUT 5    // in minutes
static uint16_t idle_timer = 0;
static uint8_t halfmin_counter = 0;
bool status = true; 

uint8_t layer;

void matrix_init_kb(void);
void matrix_scan_kb(void);
void eeconfig_init_user(void); // EEPROM is getting reset! 
void led_init_ports(void);
void led_set_kb(uint8_t usb_led);

//rgb underglow indicate layer
uint32_t layer_state_set_user(uint32_t state);
bool process_record_user(uint16_t keycode, keyrecord_t *record);

//tap dance
int cur_dance (qk_tap_dance_state_t *state);

//1. tap dance alt
void alt_finished (qk_tap_dance_state_t *state, void *user_data);
void alt_reset (qk_tap_dance_state_t *state, void *user_data);

//2. tap dance reset layer 3
void reset_finished (qk_tap_dance_state_t *state, void *user_data);
void reset_reset (qk_tap_dance_state_t *state, void *user_data);
  
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  /*
     Keymap: (Base Layer) Default Layer
   * ,--------------------------------------------------------------------,
   * |Esc | 1|  2|  3|  4|  5|  6|  7|  8|  9|  0|  -|  =|      Backsp    |
   * |--------------------------------------------------------------------|
   * |Tab  |  Q|  W|  E|  R|  T|  Y|  U|  I|  O|  P|  [|  ]|       \      |
   * |--------------------------------------------------------------------|
   * |CAPS/MO|  A|  S|  D|  F|  G|  H|  J|  K|  L|  ;|  '|      return    |
   * |--------------------------------------------------------------------|
   * |Shift ( |  Z|  X|  C|  V|  B|  N|  M|  ,|  .|  /|Shift )   | ^ | ?/ |
   * |--------------------------------------------------------------------|
   * |Ctrl|Gui |SPAlt|      Space           |  TG 1|  TG 2  | <  | V |  > |
   * `--------------------------------------------------------------------'
  */

	[_BASE] = LAYOUT_60_b_ansi(
    KC_ESC,      KC_1,      KC_2,      KC_3,      KC_4,      KC_5,      KC_6,      KC_7,      KC_8,      KC_9,      KC_0,      KC_MINS,      KC_EQL,      KC_BSPC,      KC_BSPC,      
    KC_TAB,      KC_Q,      KC_W,      KC_E,      KC_R,      KC_T,      KC_Y,      KC_U,      KC_I,      KC_O,      KC_P,      KC_LBRC,      KC_RBRC,     KC_BSLS,      
    KC_CAPS,     KC_A,      KC_S,      KC_D,      KC_F,      KC_G,      KC_H,      KC_J,      KC_K,      KC_L,      KC_SCLN,   KC_QUOT,      KC_ENT,      
    KC_LSPO,     KC_Z,      KC_X,      KC_C,      KC_V,      KC_B,      KC_N,      KC_M,      KC_COMM,   KC_DOT,    KC_RSPC,   KC_UP,        KC_SLSH,      
    KC_LCTL,     KC_LGUI,   TD(ALT_OSRAISE),   KC_SPC,    KC_SPC,    KC_SPC,    TG(_FKEYS),     TG(_CALC),     KC_LEFT,   KC_DOWN,   KC_RGHT),
  
 // Keymap: (FKeys Layer) Layer 1
 //        * ,-----------------------------------------------------------.
 //  FKEY  * |`~  | 1| 2| 3| 4| 5| 6| 7| 8| 9| 10| 11| 12 |     DEL      |
 //        * |-----------------------------------------------------------|
 //  RGB   * |RGB_LY|Tog|MOD|mod|HUE|hue|SAT|sat|VAL|val|SPD|spd|  |Print|
 //        * |-----------------------------------------------------------|
 //  mode  * |#######| Pl| Br| Ra| Sw| Sn| Kn| Xm| Gr|   |   |   |###### |
 //        * |-----------------------------------------------------------|
 //  led   * |#####   |Tog|Cyc| On|Off| +|  -| Br|   |   |#####|VOL|Mute |
 //        * |-----------------------------------------------------------|
 //        * |####|### |Alt |      #####       |####|#### |Prev|vol |Next|
 //        * `-----------------------------------------------------------'

  [_FKEYS] = LAYOUT_60_b_ansi(
    KC_GRV,      KC_F1,      KC_F2,      KC_F3,      KC_F4,      KC_F5,      KC_F6,      KC_F7,      KC_F8,      KC_F9,      KC_F10,      KC_F11,      KC_F12,      KC_DEL,      KC_DEL,      
    RGB_LYR,     RGB_TOG,    RGB_MOD,    RGB_RMOD,   RGB_HUI,    RGB_HUD,    RGB_SAI,    RGB_SAD,    RGB_VAI,    RGB_VAD,    RGB_SPI,     RGB_SPD,     KC_NO,       KC_PSCR,      
    KC_TRNS,     RGB_M_P,    RGB_M_B,    RGB_M_R,    RGB_M_SW,   RGB_M_SN,   RGB_M_K,    RGB_M_X,    RGB_M_G,    KC_NO,      KC_NO,       KC_NO,       KC_TRNS,      
    KC_TRNS,     BL_TOGG,    BL_STEP,    BL_ON,      BL_OFF,     BL_INC,     BL_DEC,     BL_BRTG,    KC_NO,      KC_NO,      KC_TRNS,     KC_VOLU,     KC_MUTE,      
    KC_TRNS,     KC_TRNS,    KC_LALT,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_MPRV,    KC_VOLD,    KC_MNXT),
	
  /*
     Keymap: (_CALC Layer) Layer 2
   * ,--------------------------------------------------------------------,
   * |### |## |## |## |## |## |## |## |## |## |## |## |## |     Backsp    |
   * |--------------------------------------------------------------------|
   * |     |  7|  8|  9|   |   |   |   | / | * | - | + |   |    ######    |
   * |--------------------------------------------------------------------|
   * |       |  4|  5|  6|   |   |   | . | , |   |   |   |      return    |
   * |--------------------------------------------------------------------|
   * |        |  1|  2|  3|   |   | = |   |   |   |   |       | ^ | RESET |
   * |--------------------------------------------------------------------|
   * |    |    |### |          0         |      |  ####  | <  | V |  >    |
   * `--------------------------------------------------------------------'
  */

  [_CALC] = LAYOUT_60_b_ansi(
    KC_TRNS,    KC_TRNS,      KC_TRNS,      KC_TRNS,      KC_TRNS,      KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_TRNS,    KC_BSPC,      KC_BSPC,      
    KC_NO,      KC_P7,        KC_P8,        KC_P9,        KC_NO,        KC_NO,      KC_NO,      KC_NO,      KC_PSLS,    KC_PAST,    KC_PMNS,    KC_PPLS,    KC_NO,      KC_TRNS,      
    KC_NO,      KC_P4,        KC_P5,        KC_P6,        KC_NO,        KC_NO,      KC_NO,      KC_PDOT,    KC_PCMM,    KC_NO,      KC_NO,      KC_NO,      KC_PENT,      
    KC_NO,      KC_P1,        KC_P2,        KC_P3,        KC_NO,        KC_NO,      KC_PEQL,    KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_UP,      KC_NO,
    KC_NO,      KC_NO,        KC_TRNS,      KC_P0,        KC_P0,        KC_P0,      KC_NO,      KC_TRNS,    KC_LEFT,    KC_DOWN,    KC_RGHT)
};

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

  if (idle_timer == 0) 
  {
    idle_timer = timer_read();
  }

  if(timer_elapsed(idle_timer) > 30000){
    halfmin_counter++;
    idle_timer = timer_read();
  }

  if(halfmin_counter >= BACKLIGHT_TIMEOUT * 2){
    status = false;
    backlight_disable();
    rgblight_mode_noeeprom(2); rgblight_sethsv_noeeprom(270,200,30);
    halfmin_counter = 0;
  }
};

void eeconfig_init_user(void) {  // EEPROM is getting reset! 
  user_config.rgb_layer_change = true; // We want this enabled by default
  eeconfig_update_user(user_config.raw); // Write default value to EEPROM now

  // use the non noeeprom versions, to write these values to EEPROM too
  rgblight_enable(); // Enable RGB by default
  rgblight_sethsv_turquoise();  // Set it to CYAN by default
  rgblight_mode(1); // set to solid by default
}

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

//COMBO FOR RESET BUTTON

const uint16_t PROGMEM rst_combo[] = {KC_BSPC, KC_PENT, COMBO_END};

combo_t key_combos[COMBO_COUNT] = {
  [BSP_PENT] = COMBO_ACTION(rst_combo)
};

void process_combo_event(uint8_t combo_index, bool pressed) {
  switch(combo_index) {
    case BSP_PENT:
      if (pressed) {
        layer = biton32(layer_state);
        if(layer == _CALC){
          reset_keyboard();
        }
      }
      break;
  }
}

uint32_t layer_state_set_user(uint32_t state) { // Runs everytime changing layer
    switch (biton32(state)) {
    case _CALC:
        if (user_config.rgb_layer_change) { rgblight_mode_noeeprom(15); rgblight_sethsv_noeeprom_coral();  } //4
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
  // Returns true if the original function still wants to be run
  // Returns false to only run the added event and cancel the original function of the key
  if(record->event.pressed){

      idle_timer = timer_read();
      halfmin_counter = 0;

    if(!status){
      layer = biton32(layer_state);
      backlight_enable();
      status = true;

      switch (layer) {
      case _CALC:
          if (user_config.rgb_layer_change) { rgblight_mode_noeeprom(15); rgblight_sethsv_noeeprom_coral();  } //4
          break;
      case _FKEYS:
          if (user_config.rgb_layer_change) { rgblight_mode_noeeprom(14); rgblight_sethsv_noeeprom(0,210,120); } //14
          break;
      default: //  for any other layers, or the default layer
          if (user_config.rgb_layer_change) { rgblight_mode_noeeprom(1); rgblight_sethsv_noeeprom_turquoise(); }
          break;
      }
    }
  }

  switch (keycode) {
    case RGB_LYR:  // This allows me to use underglow as layer indication, or as normal
        if (record->event.pressed) { 
            user_config.rgb_layer_change = 1; // Toggles the status
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

int cur_dance (qk_tap_dance_state_t *state) {
  if (state->count == 1) {
    if (state->pressed) return SINGLE_HOLD;
    else return SINGLE_TAP;
  }
  else if (state->count == 2) {
    if (state->pressed) return DOUBLE_HOLD;
    else return DOUBLE_TAP;
  }
  else if (state->count == 3) {
    if (state->interrupted || !state->pressed)  return TRIPLE_TAP;
    else return TRIPLE_HOLD;
  }
  else return 8;
}

void alt_finished (qk_tap_dance_state_t *state, void *user_data) {
  alttap_state = cur_dance(state);
  switch (alttap_state) {
    // OSL
    case SINGLE_TAP: set_oneshot_layer(_FKEYS, ONESHOT_START); clear_oneshot_layer_state(ONESHOT_PRESSED); break; 
    // ALT
    case SINGLE_HOLD: register_code(KC_LALT); break; 
    // DF
   // case DOUBLE_TAP: set_oneshot_layer(_FKEYS, ONESHOT_START); set_oneshot_layer(_FKEYS, ONESHOT_PRESSED); break; 
    // ALT + OSL + KEY
    case DOUBLE_HOLD: register_code(KC_LALT); set_oneshot_layer(_FKEYS, ONESHOT_START); clear_oneshot_layer_state(ONESHOT_PRESSED); break; 
    //Last case is for fast typing. Assuming your key is `f`:
    //For example, when typing the word `buffer`, and you want to make sure that you send `ff` and not `Esc`.
    //In order to type `ff` when typing fast, the next character will have to be hit within the `TAPPING_TERM`, which by default is 200ms.
  }
}

void alt_reset (qk_tap_dance_state_t *state, void *user_data) {
  switch (alttap_state) {
    case SINGLE_TAP: break;
    case SINGLE_HOLD: unregister_code(KC_LALT); break;
    //case DOUBLE_TAP: break;
    case DOUBLE_HOLD: unregister_code(KC_LALT); break;
  }
  alttap_state = 0;
}

qk_tap_dance_action_t tap_dance_actions[] = {
  [ALT_OSRAISE]     = ACTION_TAP_DANCE_FN_ADVANCED(NULL,alt_finished, alt_reset)
};


