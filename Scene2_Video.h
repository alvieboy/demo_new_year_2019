#ifndef __SCENE2_WILLY__
#define __SCENE2_WILLY__

#include <QMediaPlayer>
#include "Scene.h"
#include <QTimer>
#include <QImage>
#include <QMediaPlayer>
class QVideoWidget;

class Scene2_Video: public Scene
{
    Q_OBJECT;

public:
    Scene2_Video(ScreenDrawer*);
    virtual void drawTo(ScreenDrawer*) override;
    virtual void start() override;
    virtual void reset() override;
    virtual void tick() override;
public slots:
    void stateChanged(QMediaPlayer::State state);

protected:
    QMediaPlayer *player;
    ScreenDrawer *drawer;
};

#endif
