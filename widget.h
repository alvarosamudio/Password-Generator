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

    void on_pushButton_clicked();
    QString loop();

private:
    Ui::Widget *ui;

    QString generate();
    QTimer *timer;
    QString a;
    QString b;
    QString c;
    QString d;

};
#endif // WIDGET_H
