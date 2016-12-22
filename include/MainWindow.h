#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>
#include <iostream>

#include "NGLScene.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow(){}

public slots:
private:
    NGLScene *m_openGLWidget;
    QGridLayout *m_gridLayout;




};

#endif // MAINWINDOW_H
