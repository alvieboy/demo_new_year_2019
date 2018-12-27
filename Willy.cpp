#include "Willy.h"
#include <QDebug>
extern "C" const unsigned char pixel_data[];

Willy::Willy(int start_xpos, int start_ypos): QObject()
{
    xpos = target_xpos = start_xpos;
    ypos = start_ypos;
    movingbackwards=false;
}

void Willy::moveTo(int x, int y)
{
    ypos = y;
    target_xpos = x;
    if (x>xpos) {
        direction = false;
    } else if (x<xpos) {
        direction = true;
    }
}
void Willy::movetick()
{
    if (moving()) {
        if (target_xpos<xpos) {
            if (isRunning()) {
                xpos &= ~1;
                xpos-=2;
            } else {
                xpos--;
            }
        } else {
            if (isRunning()) {
                xpos &= ~1;
                xpos+=2;
            } else {
                xpos++;
            }
        }
        if (!moving()) {
            qDebug()<<"Wally in position";
            emit movementDone();
        }
    }
//    qDebug()<<"Willy "<<xpos<<","<<ypos;
}

void Willy::setPosition(int newx, int newy)
{
    xpos=newx;
    ypos=newy;
}

#define backgroundColor 0x0

void Willy::drawTo(ScreenDrawer *drawer)
{
    unsigned frame = xpos>>1;
    unsigned doff = xpos&1;

    int start = (frame&~3)*2;
    frame&=0x3;
    frame*=32;
    if ((direction & !movingbackwards) || (!direction && movingbackwards ))
    {
        frame+=1<<7;
    }
    int z;
    int i;

    const unsigned char *dstart = &pixel_data[frame];
//    qDebug()<<"Draw willy "<<start;

    for (i=0;i<16;i++)  /* 32 bytes */
    {
        unsigned char p = *dstart;
        dstart++;
        for (z=0;z<8;z++) {
            if( p&0x80)
                drawer->drawPixel( doff+start+z, i + ypos, 0xFFFFFFFF);
            p<<=1;
        }
        p = *dstart;
        dstart++;

        for (z=0;z<8;z++) {
            if (p&0x80)
            drawer->drawPixel( doff+start+8+z, i + ypos
                              , 0xFFFFFFFF);
            p<<=1;
        }
    }
}

Willy::~Willy()
{
}
