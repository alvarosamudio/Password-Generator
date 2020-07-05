#include "widget.h"
#include "ui_widget.h"
#include <QString>
#include <QDebug>
#include <QRandomGenerator64>
#include <QTimer>
#include <QStringList>
#include <QChar>

Widget::Widget(QWidget *parent): QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(loop()));
    timer->start(1000);

}

QString Widget::loop(){
    ui->horizontalSlider->setMaximum(30);
    ui->progressBar->setValue(ui->horizontalSlider->value());

    QString a("");
    QString b("");
    QString c("");
    QString d("");


    if(ui->checkBox->isChecked()){
          a = "abcdefghijklmnñopqrstuvwxyz";
    }
    else {
         a = "";
    }
    if(ui->checkBox_2->isChecked()){
         b = "ABCDEFGHIJKLMNÑOPQRSTUVWXYZ";
    }
    else {
         b = "";
    }
    if(ui->checkBox_4->isChecked()){
         c = "0123456789";
    }
    else {
         c = "";
    }
    if(ui->checkBox_3->isChecked()){
         d = "&~#{[|`^@]}¤*!:;,§/.?µ%£+°";
    }
    else {
         d = "";
    }
    return a+b+c+d;
}




QString Widget::generate(){

    QString possibleCharacters = loop();

    std::uniform_int_distribution<int> distribution(0,possibleCharacters.size());

    int taille_pass = ui->horizontalSlider->value();
    QString password;

    for(int i=0; i<taille_pass;i++)
    {
        int value = distribution(*QRandomGenerator::global());
        int index = value % possibleCharacters.length();

        QChar nextChar = possibleCharacters.at(index);

        password.append(nextChar);
    }
    return password;
}

void Widget::on_pushButton_clicked()
{
    ui->lineEdit->setText(generate());
}
