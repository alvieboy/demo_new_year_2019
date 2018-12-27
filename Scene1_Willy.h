#ifndef __SCENE1_WILLY__
#define __SCENE1_WILLY__

#include "Scene.h"
#include "Willy.h"
#include <QTimer>
#include <QImage>

class Scene1_Willy: public Scene
{
    Q_OBJECT;

public:
    Scene1_Willy();
    virtual void drawTo(ScreenDrawer*) override;
    virtual void start() override;
    virtual void reset() override;
    virtual void stop() override {}
    virtual void tick() override;
public slots:
    void willyStopped();
    void timerExpired();
protected:
    Willy willy;
    QTimer timer;
    QImage *maker_top;
    QImage *maker_bottom;
};

#endif
