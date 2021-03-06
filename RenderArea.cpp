#include "RenderArea.h"
#include <QPainter>
#include <QDebug>

#define ZOOMIDX 0
#define ZOOMMUL (1<<ZOOMIDX)

// Temporario...
#define PIXELHACK 1

RenderArea::RenderArea(QWidget *parent): QVideoWidget(parent)
{
//    setBackgroundRole(QPalette::Base);
//    setAutoFillBackground(true);
    image = new QImage(160*ZOOMMUL, 128*ZOOMMUL, QImage::Format_RGB32 );
    videomode = false;

}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(PIXELHACK+160*ZOOMMUL, 128*ZOOMMUL);
}

QSize RenderArea::sizeHint() const
{
    return QSize(PIXELHACK+160*ZOOMMUL, 128*ZOOMMUL);
}

void RenderArea::paintEvent(QPaintEvent *event)
{
    if (videomode) {
        qDebug()<<"Video";
        QVideoWidget::paintEvent(event);
    } else {
#ifdef PIXELHACK
        QRect r(1,0,1+160*ZOOMMUL,128*ZOOMMUL);
        QRect sr(0,0,160*ZOOMMUL,128*ZOOMMUL);
        QPainter painter(this);
        painter.drawImage(r,*image,sr);
#else
        QRect sr(0,0,160*ZOOMMUL,128*ZOOMMUL);
        QPainter painter(this);
        painter.drawImage(sr,*image);
#endif
    }
}

void RenderArea::drawHLine(int x, int y, int width, uint32_t color)
{
    while (width--) {
        drawPixel(x,y,color);
        x++;
    }
}

void RenderArea::drawVLine(int x, int y, int height, uint32_t color)
{
    while (height--) {
        drawPixel(x,y,color);
        y++;
    }
}

void RenderArea::drawImage(int x, int y, QImage *i)
{
    QSize s = i->size();
    x*=ZOOMMUL;
    y*=ZOOMMUL;

    QRect source(0,0,s.width(),s.height());
    QRect dest(x,y,s.width()*ZOOMMUL,s.height()*ZOOMMUL);

    QPainter painter(image);
    painter.drawImage(dest, *i, source);
}

void RenderArea::startFrame()
{
    if (!videomode)
        image->fill(0x0);
}
void RenderArea::finishFrame()
{
    if (!videomode)
        repaint();
}

void RenderArea::drawPixel(int x, int y, uint32_t color)
{
    int sx, sy;
    unsigned dx;

    if (x<0 || x>159)
        return;

    if (y<0 || y>127)
        return;

    y*=ZOOMMUL;
    for (sy=0; sy<ZOOMMUL; sy++) {
        dx=x*ZOOMMUL;
        for (sx=0;sx<ZOOMMUL;sx++) {
            image->setPixel(dx, y, color);
            dx++;
        }
        y++;
    }
}

QVideoWidget* RenderArea::getVideoWidget(){
    return this;
}

void RenderArea::setVideoMode(bool en)
{
    videomode = en;
    if (en)
        showFullScreen();
}
