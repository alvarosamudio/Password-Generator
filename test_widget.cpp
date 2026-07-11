#include <QtTest/QtTest>
#include <QCheckBox>
#include <QSlider>
#include <QLabel>
#include <QProgressBar>
#include <QApplication>

#include "widget.h"
#include "ui_widget.h"

class TestWidget : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void buildCharacterSet_lowercase();
    void buildCharacterSet_uppercase();
    void buildCharacterSet_numbers();
    void buildCharacterSet_symbols();
    void buildCharacterSet_all();
    void buildCharacterSet_none();

    void generate_validPassword();
    void generate_emptyCharset();
    void generate_zeroLength();
    void generate_length();

    void sliderChangesProgressBar();
    void sliderChangesLengthLabel();

private:
    Widget *w;
};

void TestWidget::init()
{
    w = new Widget();
}

void TestWidget::buildCharacterSet_lowercase()
{
    w->ui->checkBox->setChecked(true);
    w->ui->checkBox_2->setChecked(false);
    w->ui->checkBox_4->setChecked(false);
    w->ui->checkBox_3->setChecked(false);

    QString charset = w->buildCharacterSet();
    QCOMPARE(charset, QString("abcdefghijklmnñopqrstuvwxyz"));
}

void TestWidget::buildCharacterSet_uppercase()
{
    w->ui->checkBox->setChecked(false);
    w->ui->checkBox_2->setChecked(true);
    w->ui->checkBox_4->setChecked(false);
    w->ui->checkBox_3->setChecked(false);

    QString charset = w->buildCharacterSet();
    QCOMPARE(charset, QString("ABCDEFGHIJKLMNÑOPQRSTUVWXYZ"));
}

void TestWidget::buildCharacterSet_numbers()
{
    w->ui->checkBox->setChecked(false);
    w->ui->checkBox_2->setChecked(false);
    w->ui->checkBox_4->setChecked(true);
    w->ui->checkBox_3->setChecked(false);

    QString charset = w->buildCharacterSet();
    QCOMPARE(charset, QString("0123456789"));
}

void TestWidget::buildCharacterSet_symbols()
{
    w->ui->checkBox->setChecked(false);
    w->ui->checkBox_2->setChecked(false);
    w->ui->checkBox_4->setChecked(false);
    w->ui->checkBox_3->setChecked(true);

    QString charset = w->buildCharacterSet();
    QCOMPARE(charset, QString("&~#{[|`^@]}¤*!:;,§/.?µ%£+°"));
}

void TestWidget::buildCharacterSet_all()
{
    w->ui->checkBox->setChecked(true);
    w->ui->checkBox_2->setChecked(true);
    w->ui->checkBox_4->setChecked(true);
    w->ui->checkBox_3->setChecked(true);

    QString charset = w->buildCharacterSet();
    QString expected = QString("abcdefghijklmnñopqrstuvwxyz")
                       + "ABCDEFGHIJKLMNÑOPQRSTUVWXYZ"
                       + "0123456789"
                       + "&~#{[|`^@]}¤*!:;,§/.?µ%£+°";
    QCOMPARE(charset, expected);
}

void TestWidget::buildCharacterSet_none()
{
    w->ui->checkBox->setChecked(false);
    w->ui->checkBox_2->setChecked(false);
    w->ui->checkBox_4->setChecked(false);
    w->ui->checkBox_3->setChecked(false);

    QString charset = w->buildCharacterSet();
    QVERIFY(charset.isEmpty());
}

void TestWidget::generate_validPassword()
{
    w->ui->checkBox->setChecked(true);
    w->ui->checkBox_2->setChecked(true);
    w->ui->checkBox_4->setChecked(true);
    w->ui->checkBox_3->setChecked(true);
    w->ui->horizontalSlider->setValue(16);

    QString password = w->generate();
    QCOMPARE(password.length(), 16);

    QString charset = w->buildCharacterSet();
    for (const QChar &c : password) {
        QVERIFY2(charset.contains(c),
                 qPrintable(QString("Character '%1' not in charset").arg(c)));
    }
}

void TestWidget::generate_emptyCharset()
{
    w->ui->checkBox->setChecked(false);
    w->ui->checkBox_2->setChecked(false);
    w->ui->checkBox_4->setChecked(false);
    w->ui->checkBox_3->setChecked(false);
    w->ui->horizontalSlider->setValue(16);

    QString password = w->generate();
    QVERIFY(password.isEmpty());
}

void TestWidget::generate_zeroLength()
{
    w->ui->checkBox->setChecked(true);
    w->ui->horizontalSlider->setValue(0);

    QString password = w->generate();
    QVERIFY(password.isEmpty());
}

void TestWidget::generate_length()
{
    w->ui->checkBox->setChecked(true);
    w->ui->checkBox_2->setChecked(true);

    QList<int> lengths = {8, 16, 32, 64};
    for (int len : lengths) {
        w->ui->horizontalSlider->setValue(len);
        QString password = w->generate();
        QCOMPARE(password.length(), len);
    }
}

void TestWidget::sliderChangesProgressBar()
{
    w->ui->horizontalSlider->setValue(32);
    QCOMPARE(w->ui->progressBar->value(), 32);

    w->ui->horizontalSlider->setValue(64);
    QCOMPARE(w->ui->progressBar->value(), 64);

    w->ui->horizontalSlider->setValue(0);
    QCOMPARE(w->ui->progressBar->value(), 0);
}

void TestWidget::sliderChangesLengthLabel()
{
    w->ui->horizontalSlider->setValue(42);
    QCOMPARE(w->ui->lengthValue->text(), QString("42"));
}

void TestWidget::cleanup()
{
    delete w;
}

QTEST_MAIN(TestWidget)
#include "test_widget.moc"