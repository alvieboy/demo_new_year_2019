#include "SceneManager.h"
#include <QDebug>
#include <QTimer>

//#define FRAMEDELAY 41
#define FRAMEDELAY 1


SceneManager::SceneManager(ScreenDrawer *d): drawer(d)
{
    current_scene = scenes.end();

    /*willy.setPosition( 0, 32 );
    willy.setRunning(false);
    willy.moveTo(160,32);

    QObject::connect(&willy, SIGNAL(movementDone()),
                     this, SLOT(willyFinishedMove()));

    */
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(tick()));
    timer->setInterval(FRAMEDELAY);
    timer->start(FRAMEDELAY);
}

void SceneManager::appendScene(Scene *scene)
{
    scenes.push_back(scene);
    current_scene = scenes.end();
}

void SceneManager::nextScene()
{
    (*current_scene)->disconnect(this);
    qDebug()<<"Number of scenes: "<<scenes.size();
    std::vector<Scene*>::iterator next_scene = current_scene;
    next_scene ++;

    if (next_scene!=scenes.end()) {
        qDebug()<<"Switching to next scene";
        QObject::connect(*next_scene, SIGNAL(sceneFinished()),
                         this, SLOT(sceneFinished()));

        (*next_scene)->reset();
        (*next_scene)->start();
    } else {
        qDebug()<<"No more scenes";
    }
    current_scene = next_scene;
}

void SceneManager::sceneFinished()
{
    qDebug()<<"Scene finished";
    nextScene();
}

void SceneManager::play()
{
    current_scene = scenes.begin();

    QObject::connect(*current_scene, SIGNAL(sceneFinished()),
                     this, SLOT(sceneFinished()));

    (*current_scene)->reset();
    (*current_scene)->start();
}

void SceneManager::tick()
{
//    qDebug()<<"Start";
    drawer->startFrame();
//    qDebug()<<"Scene tick";
    if (current_scene!=scenes.end()) {
        (*current_scene)->tick();
    } else {
        //qDebug()<<"NO SCENE!";
        emit finished();
    }
    // Draw it
//    qDebug()<<"Draw";
    if (current_scene!=scenes.end()) {
        (*current_scene)->drawTo(drawer);
    }
    drawer->finishFrame();
}
