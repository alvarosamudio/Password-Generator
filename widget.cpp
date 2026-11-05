#include "widget.h"
#include "ui_widget.h"
#include <QString>
#include <QDebug>
#include <QRandomGenerator64>
#include <QStringList>
#include <QChar>
#include <QClipboard>
#include <QApplication>

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

    // QRandomGenerator::system() usa el generador criptografico del sistema
    // operativo (getrandom); global() es un PRNG sembrado, no apto para
    // contrasenas. bounded() reparte de forma uniforme, sin sesgo de modulo.
    QRandomGenerator *rng = QRandomGenerator::system();
    QString password;
    password.reserve(taille_pass);
    for (int i = 0; i < taille_pass; i++)
        password.append(possibleCharacters.at(int(rng->bounded(quint32(possibleCharacters.size())))));
    return password;
}

void Widget::on_Generar_clicked()
{
    QString password = generate();
    m_password = password;
    if (password.isEmpty())
        ui->label->setText("Selecciona al menos un tipo de carácter");
    else
        ui->label->setText(password);
}

void Widget::on_Copiar_clicked()
{
    QClipboard *clipboard = QApplication::clipboard();
    // Solo se copia una contrasena generada, nunca el texto de aviso
    if (!m_password.isEmpty())
        clipboard->setText(m_password);
}

void Widget::on_horizontalSlider_valueChanged(int value)
{
    ui->progressBar->setValue(value);
    ui->lengthValue->setText(QString::number(value));
}