// Pruebas del generador (logica) y de la ventana.
#include <QtTest/QtTest>
#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QRandomGenerator>
#include <QSlider>
#include <QSpinBox>
#include <QTextEdit>

#include "generador.h"
#include "widget.h"

class TestWidget : public QObject
{
    Q_OBJECT

private:
    static OpcionesClave solo(bool mi, bool ma, bool nu, bool si)
    {
        OpcionesClave o;
        o.minusculas = mi; o.mayusculas = ma; o.numeros = nu; o.simbolos = si;
        return o;
    }

private slots:
    // ------------------------------------------------------------ Generador

    void juego_porTipo()
    {
        QCOMPARE(Generador::juego(solo(true, false, false, false)), QString("abcdefghijklmnopqrstuvwxyz"));
        QCOMPARE(Generador::juego(solo(false, true, false, false)), QString("ABCDEFGHIJKLMNOPQRSTUVWXYZ"));
        QCOMPARE(Generador::juego(solo(false, false, true, false)), QString("0123456789"));
        QVERIFY(Generador::juego(solo(false, false, false, false)).isEmpty());
    }

    void juego_simbolosSoloAsciiSeguros()
    {
        const QString s = Generador::juego(solo(false, false, false, true));
        QVERIFY(s.size() > 20);
        for (const QChar c : s) {
            QVERIFY2(c.unicode() > 32 && c.unicode() < 127, qPrintable(QString(c)));
            QVERIFY2(!QString("\"'`\\ ").contains(c), qPrintable(QString(c)));
        }
    }

    void juego_enieYAmbiguos()
    {
        OpcionesClave o = solo(true, true, true, true);
        QVERIFY(!Generador::juego(o).contains(QChar(0xF1)));
        o.enie = true;
        QVERIFY(Generador::juego(o).contains(QStringLiteral("ñ")));
        QVERIFY(Generador::juego(o).contains(QStringLiteral("Ñ")));
        o.sinAmbiguos = true;
        for (const QChar c : QStringLiteral("Il1O0o|"))
            QVERIFY2(!Generador::juego(o).contains(c), qPrintable(QString(c)));
    }

    void generar_longitudYCaracteres()
    {
        OpcionesClave o;
        for (int n : {4, 8, 16, 64, 128}) {
            o.longitud = n;
            const QString c = Generador::generar(o);
            QCOMPARE(c.size(), n);
            const QString j = Generador::juego(o);
            for (const QChar ch : c) QVERIFY(j.contains(ch));
        }
        o.longitud = 1;      // se ajusta al minimo
        QCOMPARE(Generador::generar(o).size(), Generador::LONGITUD_MIN);
        o.longitud = 1000;   // y al maximo
        QCOMPARE(Generador::generar(o).size(), Generador::LONGITUD_MAX);
    }

    void generar_sinTiposVacia()
    {
        QVERIFY(Generador::generar(solo(false, false, false, false)).isEmpty());
    }

    void generar_unoDeCadaTipo()
    {
        OpcionesClave o;  // los 4 tipos, longitud minima: el caso mas exigente
        o.longitud = 4;
        o.unoDeCada = true;
        const QStringList g = Generador::grupos(o);
        for (int i = 0; i < 300; ++i) {
            const QString c = Generador::generar(o);
            for (const QString &grupo : g) {
                bool hay = false;
                for (const QChar ch : c) hay = hay || grupo.contains(ch);
                QVERIFY2(hay, qPrintable(c));
            }
        }
    }

    // Reparto uniforme: cada letra sale con una frecuencia parecida
    void generar_repartoUniforme()
    {
        QRandomGenerator rng(12345);
        OpcionesClave o = solo(true, false, false, false);
        o.longitud = 128;
        QHash<QChar, int> cuenta;
        const int vueltas = 400;
        for (int i = 0; i < vueltas; ++i)
            for (const QChar c : Generador::generar(o, &rng)) cuenta[c]++;
        QCOMPARE(cuenta.size(), 26);
        const double media = 128.0 * vueltas / 26;
        for (auto it = cuenta.cbegin(); it != cuenta.cend(); ++it)
            QVERIFY2(qAbs(it.value() - media) < media * 0.15, qPrintable(QString(it.key())));
    }

    void entropiaYFuerza()
    {
        OpcionesClave o = solo(false, false, true, false);
        o.longitud = 8;  // 8 * log2(10) = 26.6 bits
        QVERIFY(qAbs(Generador::entropia(o) - 26.575) < 0.01);
        QCOMPARE(Generador::fuerza(Generador::entropia(o)), Generador::Fuerza::MuyDebil);
        QCOMPARE(Generador::fuerza(50), Generador::Fuerza::Debil);
        QCOMPARE(Generador::fuerza(70), Generador::Fuerza::Aceptable);
        QCOMPARE(Generador::fuerza(100), Generador::Fuerza::Fuerte);
        QCOMPARE(Generador::fuerza(130), Generador::Fuerza::MuyFuerte);
        QCOMPARE(Generador::entropia(solo(false, false, false, false)), 0.0);
    }

    // --------------------------------------------------------------- Ventana

    void ventana_generaAlAbrirYAlCambiar()
    {
        Widget w;
        QCOMPARE(w.clave().size(), OpcionesClave{}.longitud);
        QCOMPARE(w.m_campo->toPlainText(), w.clave());

        w.m_slider->setValue(40);
        QCOMPARE(w.m_spin->value(), 40);
        QCOMPARE(w.clave().size(), 40);

        w.m_spin->setValue(20);
        QCOMPARE(w.m_slider->value(), 20);
        QCOMPARE(w.clave().size(), 20);

        w.m_minusculas->setChecked(false);
        w.m_mayusculas->setChecked(false);
        w.m_simbolos->setChecked(false);
        for (const QChar c : w.clave()) QVERIFY(c.isDigit());
    }

    void ventana_ultimoTipoNoSeDesmarca()
    {
        Widget w;
        w.m_minusculas->setChecked(false);
        w.m_mayusculas->setChecked(false);
        w.m_simbolos->setChecked(false);
        QVERIFY(w.m_numeros->isChecked());
        QVERIFY(!w.m_numeros->isEnabled());
        QVERIFY(!w.m_enie->isEnabled());       // sin letras, la ñ no aplica
        QVERIFY(!w.m_unoDeCada->isEnabled());  // un solo tipo
        w.m_simbolos->setChecked(true);
        QVERIFY(w.m_numeros->isEnabled());

        OpcionesClave nada = solo(false, false, false, false);
        w.setOpciones(nada);  // opciones guardadas invalidas: se corrige
        QVERIFY(!w.clave().isEmpty());
    }

    void ventana_copiarYBorrarPortapapeles()
    {
        Widget w;
        w.copiar();
        QClipboard *cb = QApplication::clipboard();
        QCOMPARE(cb->text(), w.clave());
        QVERIFY(w.m_borrado.isActive());
        w.borrarPortapapeles();
        QVERIFY(cb->text().isEmpty());

        // Si el usuario copio otra cosa despues, no se toca
        w.copiar();
        cb->setText(QStringLiteral("otra cosa"));
        w.borrarPortapapeles();
        QCOMPARE(cb->text(), QStringLiteral("otra cosa"));
    }
};

QTEST_MAIN(TestWidget)
#include "test_widget.moc"
