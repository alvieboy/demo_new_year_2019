#include "Scene2_Video.h"
#include <QGraphicsVideoItem>
#include <QGraphicsView>
#include <QDebug>
#include <QFile>

Scene2_Video::Scene2_Video(ScreenDrawer *w)
{
    player = new QMediaPlayer(this);
#if 0
    QGraphicsVideoItem *item = new QGraphicsVideoItem;

    QGraphicsView *graphicsView = new QGraphicsView();
    QGraphicsScene *scene = new QGraphicsScene();
    graphicsView->setScene(scene);

    graphicsView->scene()->addItem(item);
    graphicsView->show();

#endif
    //player->setVideoOutput(w);

    QString mediaPath="./resources/demo.avi";

    drawer = w;

    QFile * file = new QFile(mediaPath, this);
    if (!file->open(QIODevice::ReadOnly)) {
        qDebug()<<"CANNOOT open";
        return;
    }
    player->setMedia(QUrl::fromLocalFile(mediaPath), file);

    QObject::connect(player, SIGNAL(stateChanged(QMediaPlayer::State)),
                     this, SLOT(stateChanged(QMediaPlayer::State)));

    //void QMediaPlayer::stateChanged(QMediaPlayer::State state)
}

void Scene2_Video::drawTo(ScreenDrawer*draw)
{
}

void Scene2_Video::stateChanged(QMediaPlayer::State state)
{
    qDebug()<<"State changed"<<state;
}
void Scene2_Video::start()
{
    // WAIT FOR media status changed.
    drawer->setVideoMode(true);
    player->setVideoOutput(drawer->getVideoWidget());
    qDebug()<<"Playing";
    player->play();

}
void Scene2_Video::reset()
{
    player->stop();
}
void Scene2_Video::tick()
{
}
