#include "widget.h"
#include "ui_widget.h"
#include <QString>
#include <QDebug>
#include <QRandomGenerator64>
#include <QStringList>
#include <QChar>
#include <QClipboard>

Widget::Widget(QWidget *parent): QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->horizontalSlider->setMaximum(64);
    ui->progressBar->setMaximum(64);
    ui->progressBar->setValue(ui->horizontalSlider->value());
    ui->lengthValue->setText(QString::number(ui->horizontalSlider->value()));
}

QString Widget::buildCharacterSet()
{
    QString charset;

    if (ui->checkBox->isChecked())
        charset += "abcdefghijklmnñopqrstuvwxyz";

    if (ui->checkBox_2->isChecked())
        charset += "ABCDEFGHIJKLMNÑOPQRSTUVWXYZ";

    if (ui->checkBox_4->isChecked())
        charset += "0123456789";

    if (ui->checkBox_3->isChecked())
        charset += "&~#{[|`^@]}¤*!:;,§/.?µ%£+°";

    return charset;
}

QString Widget::generate()
{
    QString possibleCharacters = buildCharacterSet();
    int taille_pass = ui->horizontalSlider->value();

    if (possibleCharacters.isEmpty() || taille_pass == 0)
        return QString();

    std::uniform_int_distribution<int> distribution(0, possibleCharacters.size() - 1);
    QString password;

    for (int i = 0; i < taille_pass; i++) {
        int value = distribution(*QRandomGenerator::global());
        int index = value % possibleCharacters.length();
        QChar nextChar = possibleCharacters.at(index);
        password.append(nextChar);
    }
    return password;
}

void Widget::on_Generar_clicked()
{
    QString password = generate();
    if (password.isEmpty())
        ui->label->setText("Selecciona al menos un tipo de carácter");
    else
        ui->label->setText(password);
}

void Widget::on_Copiar_clicked()
{
    QClipboard *clipboard = QApplication::clipboard();
    const QString password = ui->label->text();
    clipboard->setText(password);
}

void Widget::on_horizontalSlider_valueChanged(int value)
{
    ui->progressBar->setValue(value);
    ui->lengthValue->setText(QString::number(value));
}