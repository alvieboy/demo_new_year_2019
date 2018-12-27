#ifndef __TEXTWRITER_H__
#define __TEXTWRITER_H__

#include <QObject>
#include <string.h>
#include <QTimer>
#include "font.h"
#include "ScreenDrawer.h"

class TextWriter: public QObject
{
    Q_OBJECT;
public:
    TextWriter(const char *text, uint32_t (*color)(void), int x, int y, const font_t *font);
    bool isFinished() const {return finish; }
    void drawTo(ScreenDrawer*scr);
private:
    static inline char getCharSequence(const char *t, unsigned s)
    {
        if (s>strlen(t))
            return 0;
        return t[s];
    }

public slots:
    void timerExpired();
    void cursorTimerExpired();
    void start();
signals:
    void finished();
    void charTyped();
protected:

    const char *text;
    const font_t *font;
    unsigned sequence;
    bool finish;
    uint32_t(*getcolor)(void);
    int x;
    int y;
    bool cursor_on;
    QTimer timer;
    QTimer cursor_timer;
};

#endif
