#ifndef __SCENEMANAGER_H__
#define __SCENEMANAGER_H__

#include <QObject>
#include <QTimer>
#include "Scene.h"
#include "ScreenDrawer.h"
#include <vector>

class SceneManager: public QObject
{
    Q_OBJECT;
public:
    SceneManager(ScreenDrawer*drawer);
    void appendScene(Scene *scene);
    void play();
    void nextScene();
public slots:
    void sceneFinished();
    void tick();
signals:
    void finished();

private:
    std::vector<Scene*> scenes;
    std::vector<Scene*>::iterator current_scene;
    ScreenDrawer *drawer;
    QTimer *timer;
};


#endif
