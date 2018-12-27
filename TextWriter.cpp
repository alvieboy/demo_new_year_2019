#include "TextWriter.h"
#include "DrawUtils.h"

#define TYPING_INITIAL_DELAY 2000
#define TYPING_BASE_DELAY 100
#define TYPING_RANDOM_DELAY 100


TextWriter::TextWriter(const char *itext, uint32_t (*color)(void), int ix, int iy, const font_t *ifont)
{
    text = strdup(itext);
    getcolor = color;
    x=ix;
    y=iy;
    font =ifont;
    cursor_on = false;
    sequence = 0;
    cursor_timer.setSingleShot(false);

    QObject::connect(&cursor_timer, SIGNAL(timeout()),
                     this, SLOT(cursorTimerExpired()));

    cursor_timer.start(200+(random()%500));
    finish = false;
}

void TextWriter::drawTo(ScreenDrawer*drawer)
{
    int sx=x,sy=y;
    unsigned s = 0;

    while (s<sequence) {
        char c = getCharSequence(text,s);
        if (c=='\0') {
        } else if (c=='\n') {
            sx=x;
            sy+=font->hdr.h;
        } else {
            drawChar(drawer, font, sx, sy, c, getcolor, 0);
            sx+=font->hdr.w;
            //str++;
        }
        s++;
    }
    if (cursor_on)
        drawCursor(drawer, sx, sy, font->hdr.w, font->hdr.h, getcolor);
}


void TextWriter::start()
{
    timer.setSingleShot(true);

    QObject::connect(&timer, SIGNAL(timeout()),
                     this, SLOT(timerExpired()));

    timer.start(TYPING_INITIAL_DELAY+random()%TYPING_RANDOM_DELAY);

}

void TextWriter::cursorTimerExpired()
{
  //  qDebug()<<"Cur"<<cursor_on;
    if (cursor_on==true) {
//        qDebug()<<"Inv"<<cursor_on;
        cursor_on = false;
    }
    else {
        cursor_on = true;
    }
}

void TextWriter::timerExpired()
{
    sequence++;
    if (sequence<strlen(text)) {
        timer.setSingleShot(true);
        emit charTyped();
        timer.start(TYPING_BASE_DELAY+random()%TYPING_RANDOM_DELAY);
    }
    else {
        if (!finish) {
            emit finished();
        }
        finish = true;
    }
}
