#ifndef __DRAWUTILS_H__
#define __DRAWUTILS_H__

#include "DrawUtils.h"
#include "font.h"
#include <inttypes.h>

class ScreenDrawer;

void drawCursor(ScreenDrawer *drawer,int x, int y, int w, int h, uint32_t(*getcolor)(void));
void drawChar(ScreenDrawer *drawer,
              const font_t *font,
              int x,
              int y,
              unsigned char c,
              uint32_t (*color)(void),
              uint32_t bg);

#endif
