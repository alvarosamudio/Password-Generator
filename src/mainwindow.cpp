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
    setMinimumSize(560, 500);
    resize(620, 530);

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
    OpcionesFrase f;
    f.palabras = s.value("frase/palabras", f.palabras).toInt();
    f.separador = s.value("frase/separador", f.separador).toString();
    f.mayusculas = s.value("frase/mayusculas", f.mayusculas).toBool();
    f.numero = s.value("frase/numero", f.numero).toBool();
    w->setOpcionesFrase(f);
    // El modo Comprobar no se recuerda: se empieza siempre generando
    w->setModo(s.value("modo").toInt() == Widget::ModoFrase ? Widget::ModoFrase : Widget::ModoClave);
    w->setOculta(s.value("oculta", false).toBool());
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
    const OpcionesFrase f = w->opcionesFrase();
    s.setValue("frase/palabras", f.palabras);
    s.setValue("frase/separador", f.separador);
    s.setValue("frase/mayusculas", f.mayusculas);
    s.setValue("frase/numero", f.numero);
    if (w->modo() != Widget::ModoComprobar) s.setValue("modo", int(w->modo()));
    s.setValue("oculta", w->oculta());
    DMainWindow::closeEvent(event);
}
