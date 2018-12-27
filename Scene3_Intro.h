#ifndef __SCENE3_INTRO__
#define __SCENE3_INTRO__

#include "Scene.h"
#include <QTimer>
#include <QImage>
#include "font.h"
#include <QSoundEffect>
#include "TextWriter.h"

class Scene3_Intro: public Scene
{
    Q_OBJECT;

public:
    Scene3_Intro();
    virtual void drawTo(ScreenDrawer*) override;
    virtual void start() override;
    virtual void reset() override;
    virtual void tick() override;
public slots:
    void ended();
    void charTyped();
protected:
    TextWriter *t;
    bool cursor_on;
    unsigned count;
    unsigned sequence;
    const font_t *tom;
    QTimer timer;
    bool finish;
    QList<TextWriter*> texts;
    QSoundEffect *effect;
};




#endif

