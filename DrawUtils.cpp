#include "DrawUtils.h"
#include "ScreenDrawer.h"

void drawCursor(ScreenDrawer *drawer,int x, int y, int w, int h, uint32_t(*getcolor)(void))
{
    int zx, zy;
    for (zy=0;zy<h; zy++) {
        for (zx=0;zx<w; zx++) {
            uint32_t color = getcolor();
            drawer->drawPixel(x+zx, y+zy, color);
        }
    }
}

static void drawChar16(ScreenDrawer*drawer,
                const font_t *font,
                int x, int y,
                unsigned char c,
                uint32_t (*color)(void), uint32_t bg)
{
    const uint8_t *cptr;
    //printf("Draw 16 %d %d \n", font->w, font->h);
    uint8_t hc = font->hdr.h;
    uint16_t mask = 0x8000;//(1<<(font->w-1));
    //uint16_t mask = (1<<(font->hdr.w-1));

    if ( (c<font->hdr.start) || (c>font->hdr.end)) {
        c = font->hdr.start;
    }
    cptr = (uint8_t*) ( &font->bitmap[(c - font->hdr.start)*(font->hdr.h)*2] );
    // Draw.
    do {
        uint16_t line = (uint16_t)(*cptr++)<<8;
        line += (uint16_t)*cptr++;
        //printf("Line: 0x%04x mask 0x%04x\n",line,mask);

        uint8_t wc = font->hdr.w;
        int sx=x;
        do {
            int pixel = line & mask;
            line <<=1;
            if (pixel) {
                drawer->drawPixel(x, y, color());
            } else if (0) {//bg != color) {
                drawer->drawPixel(x, y, bg);
            }
            x++;
        } while (--wc);
        x=sx;
        y++;
    } while (--hc);
}


void drawChar(ScreenDrawer *drawer,
              const font_t *font,
              int x,
              int y,
              unsigned char c,
              uint32_t (*color)(void),
              uint32_t bg)
{
    if (font->hdr.w > 8) {
        drawChar16(drawer, font, x, y, c, color, bg);
        return;
    }

    uint8_t hc = font->hdr.h;
    uint8_t *cptr;

    if ( (c<font->hdr.start) || (c>font->hdr.end)) {
        c = font->hdr.start;
    }
    cptr = &font->bitmap[(c - font->hdr.start)*(font->hdr.h)];

    // Draw.
    do {
        uint8_t line = *cptr++;
        uint8_t wc = font->hdr.w;
        int sx=x;
        do {
            int pixel = line & 0x80;
            line <<=1;
            if (pixel) {
                drawer->drawPixel(x, y, color());
            } else if (0) {//bg != color) {
                drawer->drawPixel(x, y, bg);
            }
            x++;
        } while (--wc);
        x=sx;
        y++;
    } while (--hc);
}
