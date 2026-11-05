#include "mainwindow.h"

#include <DTitlebar>
#include <QCloseEvent>
#include <QSettings>

DWIDGET_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : DMainWindow(parent)
{
    w = new Widget(this);
    setCentralWidget(w);
    titlebar()->setIcon(QIcon(QStringLiteral(":/passwordgenerator.svg")));
    titlebar()->setTitle(QString());
    setMinimumSize(540, 440);
    resize(600, 470);

    // Recuerda las opciones elegidas (nunca la contrasena)
    QSettings s;
    OpcionesClave o;
    o.longitud = s.value("longitud", o.longitud).toInt();
    o.minusculas = s.value("minusculas", o.minusculas).toBool();
    o.mayusculas = s.value("mayusculas", o.mayusculas).toBool();
    o.numeros = s.value("numeros", o.numeros).toBool();
    o.simbolos = s.value("simbolos", o.simbolos).toBool();
    o.enie = s.value("enie", o.enie).toBool();
    o.sinAmbiguos = s.value("sinAmbiguos", o.sinAmbiguos).toBool();
    o.unoDeCada = s.value("unoDeCada", o.unoDeCada).toBool();
    w->setOpciones(o);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    const OpcionesClave o = w->opciones();
    QSettings s;
    s.setValue("longitud", o.longitud);
    s.setValue("minusculas", o.minusculas);
    s.setValue("mayusculas", o.mayusculas);
    s.setValue("numeros", o.numeros);
    s.setValue("simbolos", o.simbolos);
    s.setValue("enie", o.enie);
    s.setValue("sinAmbiguos", o.sinAmbiguos);
    s.setValue("unoDeCada", o.unoDeCada);
    DMainWindow::closeEvent(event);
}
