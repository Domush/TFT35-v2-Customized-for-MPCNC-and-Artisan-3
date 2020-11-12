#include "menuSettingsFeatures.h"
#include "includes.h"

// Multi-language support
#include "Language/Language.h"

// Chip specific includes
#include "Serial.h"
#include "usart.h"

// File handling
#include "list_item.h"
#include "System/flashStorage.h"

// Menus
#include "includesMenus.h" // All menu headers

LISTITEMS featureSettingsItems = {
    // title
    LABEL_FEATURE_SETTINGS,
    // icon                 ItemType      Item Title        item value text(only for custom value)
    {
        {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
        {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
        {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
        {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
        {ICONCHAR_BACKGROUND, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
        {ICONCHAR_PAGEUP, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
        {ICONCHAR_PAGEDOWN, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND},
        {ICONCHAR_BACK, LIST_LABEL, LABEL_BACKGROUND, LABEL_BACKGROUND}
    }};

//
//setup item states
//

#define TOGGLE_NUM 2
const uint16_t toggleitem[TOGGLE_NUM] = {ICONCHAR_TOGGLE_OFF, ICONCHAR_TOGGLE_ON};

#ifdef FIL_RUNOUT_PIN
  #define ITEM_RUNOUT_NUM 3
const LABEL itemRunout[ITEM_RUNOUT_NUM] = {
    //item value text(only for custom value)
    LABEL_OFF,
    LABEL_ON,
    LABEL_SMART
};

#endif

#define ITEM_SPEED_NUM 3
const LABEL itemMoveSpeed[ITEM_SPEED_NUM] = {
    //item value text(only for custom value)
    LABEL_NORMAL_SPEED,
    LABEL_SLOW_SPEED,
    LABEL_FAST_SPEED
};
const u8 item_movespeed[ITEM_SPEED_NUM] = {LABEL_NORMAL_SPEED, LABEL_SLOW_SPEED, LABEL_FAST_SPEED};

#define ITEM_ROUTER_CONTROL 3
const LABEL itemRouterControl[ITEM_ROUTER_CONTROL] = {
    //item value text(only for custom value)
    LABEL_DISABLED,
    LABEL_M3M5,
    LABEL_FAN0
};
const u8 item_routercontrol[ITEM_ROUTER_CONTROL] = {LABEL_DISABLED, LABEL_M3M5, LABEL_FAN0};
#ifdef LED_color_PIN
  #define LED_color_NUM 9
const LABEL itemLedcolor[LED_color_NUM] = {
    //label
    LABEL_OFF,
    LABEL_WHITE,
    LABEL_RED,
    LABEL_ORANGE,
    LABEL_YELLOW,
    LABEL_GREEN,
    LABEL_BLUE,
    LABEL_INDIGO,
    LABEL_VIOLET,
};

const uint32_t led_color[LED_color_NUM] = {
    LED_OFF,
    LED_WHITE,
    LED_RED,
    LED_ORANGE,
    LED_YELLOW,
    LED_GREEN,
    LED_BLUE,
    LED_INDIGO,
    LED_VIOLET};

#endif
//
//add key number index of the items
//
typedef enum {
  SKEY_HIDEACK = 0,
  SKEY_INVERT_Y,
  SKEY_INVERT_Z,
#ifdef PS_ON_PIN
  SKEY_POWER,
#endif
#ifdef FIL_RUNOUT_PIN
  SKEY_RUNOUT,
#endif
  SKEY_SPEED,
  SKEY_ROUTER_POWER,
  SKEY_STARTGCODE,
  SKEY_ENDGCODE,
  SKEY_PERSISTENTINFO,
  SKEY_FILELIST,
#ifdef LED_color_PIN
  SKEY_KNOB,
#endif
  SKEY_COUNT   //keep this always at the end
} SKEY_LIST;

#define FE_PAGE_COUNT (SKEY_COUNT + LISTITEM_PER_PAGE - 1) / LISTITEM_PER_PAGE
int fe_cur_page = 0;

//
//set item types
//
LISTITEM settingPage[SKEY_COUNT] = {
    {ICONCHAR_TOGGLE_ON, LIST_TOGGLE, LABEL_TERMINAL_ACK, LABEL_BACKGROUND},
    {ICONCHAR_TOGGLE_ON, LIST_TOGGLE, LABEL_INVERT_YAXIS, LABEL_BACKGROUND},
    {ICONCHAR_TOGGLE_ON, LIST_TOGGLE, LABEL_INVERT_ZAXIS, LABEL_BACKGROUND},
#ifdef PS_ON_PIN
    {ICONCHAR_TOGGLE_ON, LIST_TOGGLE, LABEL_AUTO_SHUT_DOWN, LABEL_BACKGROUND},
#endif
#ifdef FIL_RUNOUT_PIN
    {ICONCHAR_BLANK, LIST_CUSTOMVALUE, LABEL_FILAMENT_RUNOUT, LABEL_OFF},
#endif
    {ICONCHAR_BLANK, LIST_CUSTOMVALUE, LABEL_MOVE_SPEED, LABEL_NORMAL_SPEED},
    {ICONCHAR_BLANK, LIST_CUSTOMVALUE, LABEL_ROUTER_CONTROL, LABEL_DISABLED},
    {ICONCHAR_TOGGLE_ON, LIST_TOGGLE, LABEL_SEND_START_GCODE, LABEL_BACKGROUND},
    {ICONCHAR_TOGGLE_ON, LIST_TOGGLE, LABEL_SEND_END_GCODE, LABEL_BACKGROUND},
    {ICONCHAR_TOGGLE_ON, LIST_TOGGLE, LABEL_PERSISTENT_STATUS_INFO, LABEL_BACKGROUND},
    {ICONCHAR_TOGGLE_ON, LIST_TOGGLE, LABEL_FILE_LISTMODE, LABEL_BACKGROUND},
#ifdef LED_color_PIN
    {ICONCHAR_BLANK, LIST_CUSTOMVALUE, LABEL_KNOB_LED, LABEL_OFF},
#endif

};

//* Perform action on button press
void updateFeatureSettings(uint8_t key_val) {
  uint8_t item_index = fe_cur_page * LISTITEM_PER_PAGE + key_val;
  switch (item_index) {
    case SKEY_HIDEACK:
      infoSettings.showResponseInTerminal = (infoSettings.showResponseInTerminal + 1) % TOGGLE_NUM;
      settingPage[item_index].icon        = toggleitem[infoSettings.showResponseInTerminal];
      featureSettingsItems.items[key_val] = settingPage[item_index];

      menuDrawListItem(&featureSettingsItems.items[key_val], key_val);
      break;

    case SKEY_INVERT_Y:
      infoSettings.invert_yaxis           = (infoSettings.invert_yaxis + 1) % TOGGLE_NUM;
      settingPage[item_index].icon        = toggleitem[infoSettings.invert_yaxis];
      featureSettingsItems.items[key_val] = settingPage[item_index];

      menuDrawListItem(&featureSettingsItems.items[key_val], key_val);
      break;

    case SKEY_INVERT_Z:
      infoSettings.invert_zaxis           = (infoSettings.invert_zaxis + 1) % TOGGLE_NUM;
      settingPage[item_index].icon        = toggleitem[infoSettings.invert_zaxis];
      featureSettingsItems.items[key_val] = settingPage[item_index];

      menuDrawListItem(&featureSettingsItems.items[key_val], key_val);
      break;

#ifdef PS_ON_PIN
    case SKEY_POWER:
      infoSettings.auto_off               = (infoSettings.auto_off + 1) % TOGGLE_NUM;
      settingPage[item_index].icon        = toggleitem[infoSettings.auto_off];
      featureSettingsItems.items[key_val] = settingPage[item_index];

      menuDrawListItem(&featureSettingsItems.items[key_val], key_val);
      break;
#endif

#ifdef FIL_RUNOUT_PIN
    case SKEY_RUNOUT:
      infoSettings.runout                 = (infoSettings.runout + 1) % ITEM_RUNOUT_NUM;
      settingPage[item_index].valueLabel  = itemRunout[infoSettings.runout];
      featureSettingsItems.items[key_val] = settingPage[item_index];

      menuDrawListItem(&featureSettingsItems.items[key_val], key_val);
      break;
#endif

    case SKEY_SPEED:
      infoSettings.move_speed             = (infoSettings.move_speed + 1) % ITEM_SPEED_NUM;
      settingPage[item_index].valueLabel  = itemMoveSpeed[infoSettings.move_speed];
      featureSettingsItems.items[key_val] = settingPage[item_index];

      menuDrawListItem(&featureSettingsItems.items[key_val], key_val);
      break;

    case SKEY_ROUTER_POWER:
      infoSettings.router_power           = (infoSettings.router_power + 1) % ITEM_ROUTER_CONTROL;
      settingPage[item_index].valueLabel  = itemRouterControl[infoSettings.router_power];
      featureSettingsItems.items[key_val] = settingPage[item_index];

      menuDrawListItem(&featureSettingsItems.items[key_val], key_val);
      break;

    case SKEY_STARTGCODE:
      infoSettings.send_start_gcode       = (infoSettings.send_start_gcode + 1) % TOGGLE_NUM;
      settingPage[item_index].icon        = toggleitem[infoSettings.send_start_gcode];
      featureSettingsItems.items[key_val] = settingPage[item_index];

      menuDrawListItem(&featureSettingsItems.items[key_val], key_val);
      break;

    case SKEY_ENDGCODE:
      infoSettings.send_end_gcode         = (infoSettings.send_end_gcode + 1) % TOGGLE_NUM;
      settingPage[item_index].icon        = toggleitem[infoSettings.send_end_gcode];
      featureSettingsItems.items[key_val] = settingPage[item_index];

      menuDrawListItem(&featureSettingsItems.items[key_val], key_val);
      break;

    case SKEY_PERSISTENTINFO:
      infoSettings.persistent_info        = (infoSettings.persistent_info + 1) % TOGGLE_NUM;
      settingPage[item_index].icon        = toggleitem[infoSettings.persistent_info];
      featureSettingsItems.items[key_val] = settingPage[item_index];

      menuDrawListItem(&featureSettingsItems.items[key_val], key_val);
      break;

    case SKEY_FILELIST:
      infoSettings.file_listmode          = (infoSettings.file_listmode + 1) % TOGGLE_NUM;
      settingPage[item_index].icon        = toggleitem[infoSettings.file_listmode];
      featureSettingsItems.items[key_val] = settingPage[item_index];

      menuDrawListItem(&featureSettingsItems.items[key_val], key_val);
      break;

#ifdef LED_color_PIN
    case SKEY_KNOB:
      infoSettings.led_color              = (infoSettings.led_color + 1) % LED_color_NUM;
      settingPage[item_index].valueLabel  = itemLedcolor[infoSettings.led_color];
      featureSettingsItems.items[key_val] = settingPage[item_index];
      ws2812_send_DAT(led_color[infoSettings.led_color]);

      menuDrawListItem(&featureSettingsItems.items[key_val], key_val);
      break;
#endif

    default:
      break;
  }
}

//* Load values on page change and reload
void loadFeatureSettings() {
  for (uint8_t i = 0; i < LISTITEM_PER_PAGE; i++) {
    uint8_t item_index = fe_cur_page * LISTITEM_PER_PAGE + i;
    switch (item_index) {
      case SKEY_HIDEACK:
        settingPage[item_index].icon  = toggleitem[infoSettings.showResponseInTerminal];
        featureSettingsItems.items[i] = settingPage[item_index];
        break;

      case SKEY_INVERT_Y:
        settingPage[item_index].icon  = toggleitem[infoSettings.invert_yaxis];
        featureSettingsItems.items[i] = settingPage[item_index];
        break;

      case SKEY_INVERT_Z:
        settingPage[item_index].icon  = toggleitem[infoSettings.invert_zaxis];
        featureSettingsItems.items[i] = settingPage[item_index];
        break;

#ifdef PS_ON_PIN
      case SKEY_POWER:
        settingPage[item_index].icon  = toggleitem[infoSettings.auto_off];
        featureSettingsItems.items[i] = settingPage[item_index];
        break;
#endif

#ifdef FIL_RUNOUT_PIN
      case SKEY_RUNOUT:
        settingPage[item_index].valueLabel = itemRunout[infoSettings.runout];
        featureSettingsItems.items[i]      = settingPage[item_index];
        break;
#endif

      case SKEY_SPEED:
        settingPage[item_index].valueLabel = itemMoveSpeed[infoSettings.move_speed];
        featureSettingsItems.items[i]      = settingPage[item_index];
        break;

      case SKEY_ROUTER_POWER:
        settingPage[item_index].valueLabel = itemRouterControl[infoSettings.router_power];
        featureSettingsItems.items[i]      = settingPage[item_index];
        break;

      case SKEY_STARTGCODE:
        settingPage[item_index].icon  = toggleitem[infoSettings.send_start_gcode];
        featureSettingsItems.items[i] = settingPage[item_index];
        break;

      case SKEY_ENDGCODE:
        settingPage[item_index].icon  = toggleitem[infoSettings.send_end_gcode];
        featureSettingsItems.items[i] = settingPage[item_index];
        break;

      case SKEY_PERSISTENTINFO:
        settingPage[item_index].icon  = toggleitem[infoSettings.persistent_info];
        featureSettingsItems.items[i] = settingPage[item_index];
        break;

      case SKEY_FILELIST:
        settingPage[item_index].icon  = toggleitem[infoSettings.file_listmode];
        featureSettingsItems.items[i] = settingPage[item_index];
        break;

#ifdef LED_color_PIN
      case SKEY_KNOB:
        settingPage[item_index].valueLabel = itemLedcolor[infoSettings.led_color];
        featureSettingsItems.items[i]      = settingPage[item_index];
        break;
#endif

      default:
        settingPage[item_index].icon  = ICONCHAR_BACKGROUND;
        featureSettingsItems.items[i] = settingPage[item_index];
        break;
    }
  }
  // set page up down button according to page count and current page
  if (SKEY_COUNT <= LISTITEM_PER_PAGE) {
    featureSettingsItems.items[5].icon = ICONCHAR_BACKGROUND;
    featureSettingsItems.items[6].icon = ICONCHAR_BACKGROUND;
  } else {
    if (fe_cur_page == 0) {
      featureSettingsItems.items[5].icon = ICONCHAR_BACKGROUND;
      featureSettingsItems.items[6].icon = ICONCHAR_PAGEDOWN;
    } else if (fe_cur_page == (FE_PAGE_COUNT - 1)) {
      featureSettingsItems.items[5].icon = ICONCHAR_PAGEUP;
      featureSettingsItems.items[6].icon = ICONCHAR_BACKGROUND;
    } else {
      featureSettingsItems.items[5].icon = ICONCHAR_PAGEUP;
      featureSettingsItems.items[6].icon = ICONCHAR_PAGEDOWN;
    }
  }
  //menuDrawListItem(&featureSettingsItems.items[5],5);
  //menuDrawListItem(&featureSettingsItems.items[6],6);
}

void menuSettingsFeatures(void) {
  KEY_VALUES key_num = KEY_IDLE;
  SETTINGS now       = infoSettings;
  fe_cur_page        = 0;
  loadFeatureSettings();
  menuDrawListPage(&featureSettingsItems);

  while (infoMenu.menu[infoMenu.active] == menuSettingsFeatures) {
    key_num = menuKeyGetValue();
    switch (key_num) {
      case KEY_ICON_5:
        if (FE_PAGE_COUNT > 1) {
          if (fe_cur_page > 0) {
            fe_cur_page--;
            loadFeatureSettings();
            menuRefreshListPage();
          }
        }
        break;

      case KEY_ICON_6:
        if (FE_PAGE_COUNT > 1) {
          if (fe_cur_page < FE_PAGE_COUNT - 1) {
            fe_cur_page++;
            loadFeatureSettings();
            menuRefreshListPage();
          }
        }
        break;

      case KEY_ICON_7:
        infoMenu.active--;
        break;
      default:
        if (key_num < LISTITEM_PER_PAGE) {
          updateFeatureSettings(key_num);
          menuDrawListItem(&featureSettingsItems.items[key_num], key_num);
        }
        break;
    }

    runUpdateLoop();
  }

  if (memcmp(&now, &infoSettings, sizeof(SETTINGS))) {
    storePara();
  }
}