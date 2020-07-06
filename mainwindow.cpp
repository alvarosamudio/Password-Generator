#include "mainwindow.h"
#include <DMainWindow>

DWIDGET_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    w = new Widget;
    setMinimumSize(600,300);
    setMaximumSize(600,300);
    setCentralWidget(w);

}

MainWindow::~MainWindow()
{

}


