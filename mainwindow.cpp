#include "mainwindow.h"
#include <DMainWindow>

DWIDGET_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    w = new Widget;
    setMinimumSize(470,360);
    setMaximumSize(470,360);
    setCentralWidget(w);

}

MainWindow::~MainWindow()
{

}


