#include "font.h"
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <fcntl.h>

extern font_t tom_thumb_font;

font_t *fonts = &tom_thumb_font;

#ifndef ALIGN
#define ALIGN(x, alignment) (((x)+(alignment-1)) & ~(alignment-1))
#endif

const font_t * font_find(const char *name)
{
    font_t *font;
    int f;

    for (font = fonts; font; font=font->next) {
        if (strcmp(name,font->name)==0)
            return font;
    }
    char path[128];
    sprintf(path,"resources/%s", name);
    f=open(path, O_RDONLY);

    if (f>=0) {
        font = malloc(sizeof(font_t));
        read(f, &font->hdr, sizeof(font->hdr));

        unsigned bpp = ALIGN((font->hdr.w-1),8) / 8;
        bpp *= ((font->hdr.end - font->hdr.start)+1);
        bpp *= font->hdr.h;
        strncpy(font->name,name,sizeof(font->name));
        printf("Loaded font %s %d x %d\n", font->name,font->hdr.w,font->hdr.h);

        font->bitmap = malloc(bpp);
        if (font->bitmap!=NULL) {
            read(f, font->bitmap, bpp);
            // Link it
            font->next = fonts;
            fonts = font;
        } else {
            free(font);
            font = NULL;
        }
    } else {
        free(font);
        font = NULL;
    }

    if (NULL==font) {
        printf("Cannot find or alloc font '%s', reverting to 'thumb'\n", name);
        return &tom_thumb_font;
    }
    return font;
}
