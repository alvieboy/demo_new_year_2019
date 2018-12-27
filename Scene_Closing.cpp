#include "Scene_Closing.h"
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




Scene_Closing::Scene_Closing(ScreenDrawer *d)
{

    const font_t *font2 = font_find("6x10");
                     //"------------------\n"//
    t = new TextWriter("$ cat > hny.c\n"
                       "#include <stdio.h>\n"
                       "int main(void)\n"
                       "{\n"
                       "  puts(\"Happy \"\n"
                       "    \"new Year!\");\n"
                       "  return 0;\n"
                       "}\n"
                       "^D\n"
                       "$ gcc hny.c -o hny\n"
                       "$ ./hny\n"
                       "Happy new Year!\n"
                       "$ ",



                       &getcolor_green, BLOCKSIZE*BLOCKS_X+4, 0, font2);

    QObject::connect(t,  SIGNAL(charTyped()), this, SLOT(charTyped()));

    tdrawer = new TetrisScreenDrawer(d);

    QObject::connect(&timer,  SIGNAL(timeout()), this, SLOT(ended()));

    QObject::connect(t,  SIGNAL(finished()), this, SLOT(textEnded()));

}

void Scene_Closing::textEnded()
{
    timer.setSingleShot(true);
    timer.start(4000);
}

void Scene_Closing::drawTo(ScreenDrawer*drawer)
{
    // Draw Vline
    drawer->setVideoMode(false);
    uint32_t border = getcolor_amber();
    drawer->drawVLine((BLOCKS_X*BLOCKSIZE)+1, 0, 128, border);
    drawer->drawVLine(0, 0, 128, border);
    drawer->drawHLine(0, 0, (BLOCKS_X*BLOCKSIZE)+1, border);
    drawer->drawHLine(0, 127, (BLOCKS_X*BLOCKSIZE)+1, border);
    tetris.game_loop(tdrawer);

    t->drawTo(drawer);

}

void Scene_Closing::start()
{
    tetris.setup_game();
    t->start();
}

void Scene_Closing::reset()
{
}

void Scene_Closing::charTyped()
{
}
void Scene_Closing::tick()
{
    tetris.game_event_check(tdrawer);
//    t->tick();
}

void Scene_Closing::ended()
{
    emit sceneFinished();
}

void Scene_Closing::stop()
{
    delete t;
}

