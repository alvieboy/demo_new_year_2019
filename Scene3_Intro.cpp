#include "Scene3_Intro.h"
#include <QDebug>
#include "DrawUtils.h"
#include "TextWriter.h"
#include <QFile>

static inline uint8_t noisy(uint8_t base)
{
    uint32_t noise;
    noise = random() & 0x3F;
    noise += base;
    if (noise>0xff)
        noise=0xff;
    return noise;
}

static uint32_t getcolor_green(void)
{
    return ((uint32_t)noisy(0)<<16) + ((uint32_t)noisy(0xB0)<<8) + uint32_t(noisy(0));
}

static uint32_t getcolor_amber(void)
{
    return ((uint32_t)noisy(0xB6)<<16) + ((uint32_t)noisy(0x88)<<8) + uint32_t(noisy(0));
}

static uint32_t getcolor_bluish(void)
{
    return ((uint32_t)noisy(0x00)<<16) + ((uint32_t)noisy(0x88)<<8) + uint32_t(noisy(0xb6));
}




Scene3_Intro::Scene3_Intro()
{
    finish = false;
    const font_t *tom = font_find("thumb");
    const font_t *font2 = font_find("6x10");
    const font_t *font3 = font_find("16x16");

    //QObject::connect(&timer, SIGNAL(timeout()),
    //                 this, SLOT(timerExpired()));

    t = new TextWriter("Alvaro Lopes Productions\napresenta...", &getcolor_green, 8, 0, font2);
    texts.push_back(t);
    TextWriter *t2 = new TextWriter("Uma demonstracao de final de ano para os\n"
                                    "makers de Portugal. Que o proximo ano \n"
                                    "traga muitas coisas de montar e soldar\n"
                                    "e tambem um Osciloscopio novo a estrear.", &getcolor_amber, 0, 24, tom);
    texts.push_back(t2);
    QObject::connect(t, SIGNAL(finished()), t2, SLOT(start()));

    TextWriter *t3 = new TextWriter("Um bom ano\n de 2019\npara todos!", &getcolor_bluish, 0, 64, font3);
    QObject::connect(t2, SIGNAL(finished()), t3, SLOT(start()));
    texts.push_back(t3);


    QObject::connect(t,  SIGNAL(charTyped()), this, SLOT(charTyped()));
    QObject::connect(t2, SIGNAL(charTyped()), this, SLOT(charTyped()));
    QObject::connect(t3, SIGNAL(charTyped()), this, SLOT(charTyped()));

    effect = new QSoundEffect();

    effect->setSource(QUrl::fromLocalFile("resources/typewriter2.wav"));
    effect->setVolume(1.0f);
    effect->play();
}



void Scene3_Intro::drawTo(ScreenDrawer*drawer)
{
    QList<TextWriter*>::iterator i;
    for (i=texts.begin(); i!=texts.end();i++) {
        (*i)->drawTo(drawer);
    }

}

void Scene3_Intro::start()
{
    t->start();
}

void Scene3_Intro::reset()
{
}

void Scene3_Intro::charTyped()
{
    qDebug()<<"Play";
    if (!effect->isPlaying())
        effect->play();
}

void Scene3_Intro::tick()
{
    bool allfinished = true;
    QList<TextWriter*>::iterator i;
    for (i=texts.begin(); i!=texts.end();i++) {
        if (!(*i)->isFinished()) {
            allfinished=false;
        }
    }
    if (allfinished && !finish) {
        finish=true;
        timer.setSingleShot(true);
        QObject::connect(&timer, SIGNAL(timeout()),
                         this, SLOT(ended()));
        timer.start(2000);

    }
}

void Scene3_Intro::ended()
{
    emit sceneFinished();
}



