#include "Scene1_Willy.h"
#include <QDebug>

#define WILLY_MOVE_CENTER 0
#define WILLY_WAIT1       1
#define WILLY_MOVE_EDGE   2
#define WILLY_WAIT2       3
#define WILLY_PULL1       4
#define WILLY_MOVE_FASTEDGE       5
#define WILLY_WAIT3       6
#define WILLY_PULL2       7
#define WILLY_END         8

#define WILLY_VPOS 32
#define WILLY_VPOS2 16+64

#define ROPE_COLOR 0x3fa1d9
static int localscene = WILLY_MOVE_CENTER;

void Scene1_Willy::drawTo(ScreenDrawer*drawer) {

    //if (localscene==WILLY_END)
    //    return;

    if (maker_top && localscene==WILLY_PULL1) {
        int imgx = willy.getX()+16;
        if (imgx<0)
            imgx=0;

        drawer->drawImage(imgx, 0, maker_top);
        // Draw line.
        drawer->drawHLine(willy.getX()+7, WILLY_VPOS+11, 38, ROPE_COLOR);//0xFFFFFFFF);
    }

    if (maker_top && localscene>WILLY_PULL1) {
        drawer->drawImage(0, 0, maker_top);

        if (localscene==WILLY_PULL2) {

            int imgx = willy.getX()+16;
            if (imgx<0)
                imgx=0;

            drawer->drawImage(imgx, 64, maker_bottom);

            // Draw line.
            drawer->drawHLine(willy.getX()+7, WILLY_VPOS2+11, 38, ROPE_COLOR);//0xFFFFFFFF);
        }
    }
    if (maker_top && localscene>WILLY_PULL2) {

            drawer->drawImage(0, 64, maker_bottom);
    }
    willy.drawTo(drawer);
}
void Scene1_Willy::start() {
    localscene = WILLY_MOVE_CENTER;

    QObject::connect(&willy, SIGNAL(movementDone()),
                     this, SLOT(willyStopped()));

    QObject::connect(&timer, SIGNAL(timeout()),
                     this, SLOT(timerExpired()));

    willy.moveTo(64,WILLY_VPOS);

}

void Scene1_Willy::willyStopped()
{
    switch (localscene) {
    case WILLY_MOVE_CENTER:
        timer.setSingleShot(true);
        timer.start(1000);
        localscene = WILLY_WAIT1;
        break;
    case WILLY_MOVE_EDGE:
        timer.setSingleShot(true);
        timer.start(500);
        localscene = WILLY_WAIT2;
        break;
    case WILLY_PULL1:
        localscene = WILLY_MOVE_FASTEDGE;
        willy.setPosition(-8, WILLY_VPOS2);
        willy.setMoveBackwards(false);
        willy.setRunning(true);
        willy.moveTo(160-8, WILLY_VPOS2);
        break;
    case WILLY_MOVE_FASTEDGE:
        timer.setSingleShot(true);
        timer.start(500);
        localscene = WILLY_WAIT3;
        break;
    case WILLY_PULL2:
        timer.setSingleShot(true);
        timer.start(4000);
        localscene = WILLY_END;
        break;

    default:
        break;
    }
}
void Scene1_Willy::timerExpired()
{
    switch (localscene) {
    case WILLY_WAIT1:
        // Move to edge
        willy.setRunning(true);
        willy.moveTo( 160-8, 32);
        localscene = WILLY_MOVE_EDGE;
        break;
    case WILLY_WAIT2:
        willy.setRunning(false);
        willy.setMoveBackwards(true);
        willy.moveTo( -(16+38), 32);
        localscene = WILLY_PULL1;
        break;
    case WILLY_WAIT3:
        willy.setRunning(false);
        willy.setMoveBackwards(true);
        willy.moveTo( -(16+38), 16+64);
        localscene = WILLY_PULL2;
        break;
    case WILLY_END:
        sceneFinished();
    default:
        break;
    }
}

void Scene1_Willy::reset() {
    willy.setPosition(160,32);
}
void Scene1_Willy::tick() {
    willy.movetick();
}

Scene1_Willy::Scene1_Willy()
{
    maker_top = new QImage();
    maker_bottom = new QImage();
    if (maker_top->load("resources/maker1_top.png")==false) {
        qDebug()<<"Could not load maker1_top.png";
        delete (maker_top);
        maker_top = NULL;
    }

    if (maker_bottom->load("resources/maker1_bottom.png")==false) {
        qDebug()<<"Could not load maker1_bottom.png";
        delete (maker_bottom);
        maker_bottom = NULL;
    }

}
