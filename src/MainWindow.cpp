#include "MainWindow.h"
#include <QFileDialog>
#include <QPushButton>
#include <QDesktopServices>
#include <QGLFormat>
#include <QGroupBox>
#include <QPushButton>
#include <QFileDialog>
#include <QLabel>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent){
    QGroupBox *gb = new QGroupBox(this);
    setCentralWidget(gb);
    m_gridLayout = new QGridLayout(gb);
    gb->setLayout(m_gridLayout);

    QGLFormat format;
    format.setVersion(4,1);
    format.setProfile(QGLFormat::CoreProfile);

    //add our openGL context to our scene
    m_openGLWidget = new NGLScene(format,this);
    m_gridLayout->addWidget(m_openGLWidget,0,0,4,4);
    m_openGLWidget->show();
}

