#include "mainwindow.h"
#include "RenderArea.h"
#include <QVBoxLayout>
#include "SceneManager.h"
#include "Scene1_Willy.h"
#include "Scene2_Video.h"
#include "Scene3_Intro.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

    renderarea = new RenderArea(this);

    //QVBoxLayout *mainLayout = new QVBoxLayout;
    setCentralWidget(renderarea);
    //setBackgroundRole(QPalette::Base);
    //setAutoFillBackground(true);
    //    mainLayout->show();
    //    setLayout(mainLayout);
    manager = new SceneManager(renderarea);

    //manager->appendScene(new Scene3_Intro());
    manager->appendScene(new Scene1_Willy());
    manager->appendScene(new Scene2_Video(renderarea));
    manager->play();

    QObject::connect(manager, SIGNAL(finished()),
                     this, SLOT(close()));

}

MainWindow::~MainWindow()
{
}
