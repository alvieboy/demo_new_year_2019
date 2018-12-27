#ifndef __WILLY_H__
#define __WILLY_H__
#include <QObject>
#include <QPoint>
#include "ScreenDrawer.h"

class Willy: public QObject
{
    Q_OBJECT;
public:
    Willy(int start_xpos=0, int start_ypos=0);
    ~Willy();
    void drawTo(ScreenDrawer *drawer);
    void setPosition(int xpos, int ypos);
    void movetick();
    void moveTo(int new_xpos, int new_ypos);
    bool moving() const { return target_xpos!=xpos; }
    void setRunning(bool r) { running=r; };
    bool isRunning() const { return running; }
    void setMoveBackwards(bool b) { movingbackwards=b; }
    QPoint getPosition() const { return QPoint(xpos,ypos); }
    int getX() const { return xpos; }
signals:
    void movementDone(void);
protected:
    int xpos, ypos;
    int target_xpos;
    bool direction;
    bool movingbackwards;
    bool running;
};

#endif
