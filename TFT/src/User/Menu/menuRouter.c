#include "menuRouter.h"
#include "includes.h"

// LCD init functions
#include "GUI.h"

// Multi-language support
#include "Language/Language.h"

// Chip specific includes
#include "Serial.h"
#include "usart.h"

// USB drive support (select TFT models)
#include "usbh_usr.h"

// File handling
#include "list_item.h"

// Gcode processing
#include "Gcode/gcodeSender.h"

// Timing functions
#include "System/boot.h"

// Menus
#include "includesMenus.h"  // All menu headers

//1 title, ITEM_PER_PAGE items(icon+label)
const MENUITEMS routerItems = {
    //   title
    LABEL_ROUTER,
    // icon                       label
    {
        {ICON_DEC, LABEL_DEC},
        {ICON_BACKGROUND, LABEL_BACKGROUND},
        {ICON_BACKGROUND, LABEL_BACKGROUND},
        {ICON_INC, LABEL_INC},
        {ICON_ROUTER_OFF, LABEL_ROUTER_OFF},
        {ICON_CHANGE_BIT, LABEL_CHANGE_BIT},
        {ICON_ROUTER_ON, LABEL_ROUTER_ON},
        {ICON_BACK, LABEL_BACK}}};

const char* routerID  = ROUTER_ID;
const u8 routerMaxPWM = ROUTER_MAX_PWM;

extern JOBSTATUS infoJobStatus;

/**
 * routerControl.
 *
 * @version	v1.0.0	Thursday, February 27th, 2020.
 * @global
 * @param	u8	speed	// 0-255
 * @return	void
 */
void routerControl(u8 speed) {
  if (speed > 255) {
    speed = 255;
  }
  switch (infoSettings.router_power) {
    case 1:
      if (!speed || speed < 1) {
        if (infoJobStatus.isM0Paused) {
          sendCommand(SERIAL_PORT, "M5\n");
        } else {
          queueCommand(false, "M5\n");
        }
      } else {
        if (infoJobStatus.isM0Paused) {
          sendCommand(SERIAL_PORT, "M3 S%d\n", speed);
        } else {
          queueCommand(false, "M3 S%d\n", speed);
        }
      }
      infoJobStatus.routerSpeed = speed;
      break;

    case 2:
      if (!speed || speed < 1) {
        if (infoJobStatus.isM0Paused) {
          sendCommand(SERIAL_PORT, "M107\n");
        } else {
          queueCommand(false, "M107\n");
        }
      } else {
        if (infoJobStatus.isM0Paused) {
          sendCommand(SERIAL_PORT, "M106 S%d\n", speed);
        } else {
          queueCommand(false, "M106 S%d\n", speed);
        }
      }
      infoJobStatus.routerSpeed = speed;
      break;

    default:
      break;
  }
}
void routerChangeBit(void) {
  u8 speed = infoJobStatus.routerSpeed;
  if (speed > 0) {
    routerControl(0);
  }
  if (infoJobStatus.coordSpace < 53) infoJobStatus.coordSpace = 53;
  queueCommand(false, "G59\n");
  queueCommand(false, "G0 X20 Y200 Z150 F%d\n", SPEED_MOVE_FAST);
  queueCommand(false, "M0 Please replace the bit. Continue when finished.");
  infoJobStatus.isM0Paused = true;
}

void routerShowSpeed(void) {
  const GUI_RECT rect = {exhibitRect.x0, CENTER_Y - BYTE_HEIGHT, exhibitRect.x1, CENTER_Y};
  u8 fs;
  fs = (infoJobStatus.routerSpeed * 100) / 255;
  GUI_ClearRect(rect.x0, rect.y0, rect.x1, rect.y1);
  GUI_DispStringInPrect(&rect, (u8*)routerID);
  char router_s[5];
  sprintf(router_s, "%3d%%", fs);
  GUI_DispString(CENTER_X - BYTE_WIDTH, CENTER_Y, (u8*)router_s);
}

void routerSpeedReDraw(void) {
  char router_s[5] = "";
  sprintf(router_s, "%3d%%", (infoJobStatus.routerSpeed * 100) / 255);
  GUI_DispString(CENTER_X - BYTE_WIDTH, CENTER_Y, (u8*)router_s);
}

void menuRouter(void) {
  u8 nowRouterSpeed  = infoJobStatus.routerSpeed;
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&routerItems);
  routerShowSpeed();
  while (infoMenu.menu[infoMenu.active] == menuRouter) {
    key_num = menuKeyGetValue();
    switch (key_num) {
      case KEY_ICON_0:  // Router speed --
        if (infoJobStatus.routerSpeed > 0) {
          if ((infoJobStatus.routerSpeed - 2) > 0) {
            infoJobStatus.routerSpeed -= 2;  //2.55 is 1 percent, rounding down
            timedMessage(2, TIMED_INFO, "Reducing router speed");
          } else {
            infoJobStatus.routerSpeed = 0;
            timedMessage(2, TIMED_INFO, "Turning off router");
          }
        }
        break;

      case KEY_ICON_3:  // Router speed ++
        if (infoJobStatus.routerSpeed < routerMaxPWM) {
          if (infoJobStatus.routerSpeed + 2 <= routerMaxPWM) {
            infoJobStatus.routerSpeed += 2;  //2.55 is 1 percent, rounding down
            timedMessage(2, TIMED_INFO, "Increasing router speed");
          } else {
            infoJobStatus.routerSpeed = routerMaxPWM;
            timedMessage(2, TIMED_INFO, "Turning off router");
          }
        }
        break;

      case KEY_ICON_4:  // Router off
        if (jobInProgress() && !jobIsPaused()) {
          popupReminder((u8*)"Not allowed - Bit damage likely", (u8*)"You must first pause the CNC job.");
        } else {
          timedMessage(2, TIMED_INFO, "Turning off router");
          routerControl(0);
        }
        break;

      case KEY_ICON_5:  // Swap bits
        setPaused(true);
        infoMenu.menu[infoMenu.active] = menuChangeBit;
        break;

      case KEY_ICON_6:  // Router on
        timedMessage(3, TIMED_INFO, "Powering up router");
        routerControl(routerMaxPWM);
        break;

      case KEY_ICON_7:  // Back
        infoMenu.active--;
        break;

      default:
        break;
    }
    if (nowRouterSpeed != infoJobStatus.routerSpeed) {
      routerControl(infoJobStatus.routerSpeed);
      nowRouterSpeed = infoJobStatus.routerSpeed;
      routerSpeedReDraw();
    }
    runUpdateLoop();
  }
}
