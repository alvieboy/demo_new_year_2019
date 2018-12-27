QT += widgets multimedia multimediawidgets opengl

TARGET = rgbvideodemo

HEADERS=mainwindow.h RenderArea.h Willy.h Scene.h SceneManager.h \
	Scene1_Willy.h Scene2_Video.h Scene3_Intro.h font.h DrawUtils.h TextWriter.h
SOURCES=rgbvideodemo.cpp mainwindow.cpp RenderArea.cpp willy.c Willy.cpp Scene.cpp SceneManager.cpp \
	Scene1_Willy.cpp Scene2_Video.cpp font.c Scene3_Intro.cpp tomthumb.c DrawUtils.cpp TextWriter.cpp


#LIBS+="-L/usr/lib/x86_64-linux-gnu/mesa"
