#ifndef __SCENE_H__
#define __SCENE_H__

#include "ScreenDrawer.h"
#include <QObject>

class Scene: public QObject
{
    Q_OBJECT;
public:
    virtual void drawTo(ScreenDrawer*) = 0;
    virtual void start() = 0;
    virtual void reset() = 0;
    virtual void stop() = 0;
    virtual void tick() = 0;
signals:
    void sceneFinished();
};

#endif

