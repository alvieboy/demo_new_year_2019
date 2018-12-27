#ifndef __SCENE_CLOSING__
#define __SCENE_CLOSING__

#include "Scene.h"
#include <QTimer>
#include <QImage>
#include "font.h"
#include <QSoundEffect>
#include "TextWriter.h"
#include "tetris.h"

class TetrisScreenDrawer: public ScreenDrawer
{
public:
    TetrisScreenDrawer(ScreenDrawer*s): drawer(s) {
    }
    void drawPixel(int x, int y, uint32_t color)
    {
        drawer->drawPixel(y,x,color);
    }
    void drawImage(int x, int y, QImage *)  {}
    void drawHLine(int x, int y, int width, uint32_t color) { }
    void drawVLine(int x, int y, int width, uint32_t color) { }
    void startFrame() {};
    void finishFrame() { };
    QVideoWidget* getVideoWidget() { return NULL; }
    void setVideoMode(bool) { }
private:
    ScreenDrawer *drawer;
};

class Scene_Closing: public Scene
{
    Q_OBJECT;

public:
    Scene_Closing(ScreenDrawer*);
    virtual void drawTo(ScreenDrawer*) override;
    virtual void start() override;
    virtual void stop() override;
    virtual void reset() override;
    virtual void tick() override;
public slots:
    void ended();
    void charTyped();
    void textEnded();
protected:
    TextWriter *t;
    bool cursor_on;
    unsigned count;
    unsigned sequence;
    const font_t *tom;
    QTimer timer;
    bool finish;
    QSoundEffect *effect;
    Tetris tetris;
    TetrisScreenDrawer *tdrawer;
};




#endif

