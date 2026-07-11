#include "mainwindow.h"
#include <DMainWindow>

DWIDGET_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    w = new Widget;
    setMinimumSize(500,250);
    setCentralWidget(w);

}

MainWindow::~MainWindow()
{

}


