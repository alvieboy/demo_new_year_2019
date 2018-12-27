#include "mainwindow.h"
#include "RenderArea.h"
#include <QVBoxLayout>
#include "SceneManager.h"
#include "Scene1_Willy.h"
#include "Scene2_Video.h"
#include "Scene3_Intro.h"
#include "Scene_Closing.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::Window | Qt::FramelessWindowHint)
{

    renderarea = new RenderArea(this);

    //QVBoxLayout *mainLayout = new QVBoxLayout;
    //layout()->addWidget(renderarea);

    //QVBoxLayout *layout = new QVBoxLayout();

    //layout->addWidget(renderarea);

    //setLayout(layout);
    setCentralWidget(renderarea);
    renderarea->show();

    //setBackgroundRole(QPalette::Base);
    //setAutoFillBackground(true);
    //    mainLayout->show();
    //    setLayout(mainLayout);
    manager = new SceneManager(renderarea);

    manager->appendScene(new Scene3_Intro());
    manager->appendScene(new Scene1_Willy());
    manager->appendScene(new Scene_Closing(renderarea));
    manager->appendScene(new Scene2_Video(renderarea));
    manager->play();

    QObject::connect(manager, SIGNAL(timeout()),
                     this, SLOT(close()));
}




MainWindow::~MainWindow()
{
}
