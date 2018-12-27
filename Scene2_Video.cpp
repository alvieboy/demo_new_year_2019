#include "Scene2_Video.h"
#include <QGraphicsVideoItem>
#include <QGraphicsView>
#include <QDebug>
#include <QFile>

Scene2_Video::Scene2_Video(ScreenDrawer *w)
{
    //void QMediaPlayer::stateChanged(QMediaPlayer::State state)
    drawer = w;
}

void Scene2_Video::drawTo(ScreenDrawer*draw)
{
}

void Scene2_Video::stateChanged(QMediaPlayer::State state)
{
    qDebug()<<"State changed"<<state;
    if (state==QMediaPlayer::StoppedState) {
        emit sceneFinished();
    }
}
void Scene2_Video::start()
{
    player = new QMediaPlayer(this);
    QString mediaPath="./resources/newyear.avi";

    QFile * file = new QFile(mediaPath, this);
    if (!file->open(QIODevice::ReadOnly)) {
        qDebug()<<"CANNOOT open";
        return;
    }
    player->setMedia(QUrl::fromLocalFile(mediaPath), file);

    QObject::connect(player, SIGNAL(stateChanged(QMediaPlayer::State)),
                     this, SLOT(stateChanged(QMediaPlayer::State)));


    // WAIT FOR media status changed.
    drawer->setVideoMode(true);
    player->setVideoOutput(drawer->getVideoWidget());
    qDebug()<<"Playing";
    player->play();

}

void Scene2_Video::stop()
{
    player->stop();
    //    player->destroy();
    //delete player;
    //    player->setVideoOutput(NULL);//drawer->getVideoWidget());
}
void Scene2_Video::reset()
{
    //player->stop();
}
void Scene2_Video::tick()
{
}
