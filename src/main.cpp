#include <QApplication>
#include "MainWindow.h"

int main(int argc, char **argv)
{
    QApplication app(argc,argv);
    MainWindow w;
    w.setWindowTitle(QString("Arbiturary Fluid Sim"));
    w.resize(750,480);
    w.show();
    app.exec();
}



