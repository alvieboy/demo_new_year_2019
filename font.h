#ifndef __FONT_H__
#define __FONT_H__

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef struct {
    uint8_t w;
    uint8_t h;
    uint8_t start;
    uint8_t end;
} __attribute__((packed)) fonthdr_t;

typedef struct font {
    fonthdr_t hdr;
    char name[10];
    uint8_t *bitmap;
    uint8_t ref;
    struct font *next;
} font_t;


const font_t *font_find(const char *name);

#ifdef __cplusplus
}
#endif

#endif
