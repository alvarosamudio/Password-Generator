#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);

private slots:
    void on_Generar_clicked();
    void on_Copiar_clicked();
    void on_horizontalSlider_valueChanged(int value);

public:
    Ui::Widget *ui;
    QString generate();
    QString buildCharacterSet();

private:
    QString m_password;  // ultima contrasena generada (lo que se copia)

};
#endif // WIDGET_H
