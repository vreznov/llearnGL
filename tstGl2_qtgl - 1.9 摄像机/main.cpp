#include "mainwindow.h"

#include <QApplication>
#include "kglwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    KGLWidget window;
    window.show();

    return a.exec();
}
