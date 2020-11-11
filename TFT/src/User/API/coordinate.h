#ifndef _COORDINATE_H_
#define _COORDINATE_H_
#include "includes.h"

typedef enum {
  X_AXIS = 0,
  Y_AXIS,
  Z_AXIS,
  E_AXIS,
  TOTAL_AXIS
} AXIS;

typedef struct
{
  float axis[TOTAL_AXIS];
  u32 gantryspeed;
} COORDINATE;

COORDINATE pauseCoords;

extern const char axis_id[TOTAL_AXIS];

bool coorGetRelative(void);
void coorSetRelative(bool mode);
bool eGetRelative(void);
void eSetRelative(bool mode);
bool coordinateIsClear(void);
void coordinateSetClear(bool clear);
float coordinateGetAxisTarget(AXIS axis);
void coordinateSetAxisTarget(AXIS axis, float position);
u32 coordinateGetGantrySpeed(void);
void coordinateSetGantrySpeed(u32 gantryspeed);
void coordinateGetAll(COORDINATE *tmp);
float coordinateGetAxisActual(AXIS axis);
void coordinateSetAxisActualSteps(AXIS axis, int steps);

#endif
