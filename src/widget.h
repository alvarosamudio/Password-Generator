#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include "generador.h"

class QCheckBox;
class QComboBox;
class QStackedWidget;
class QAbstractButton;
namespace Dtk { namespace Widget { class DButtonBox; class DButtonBoxButton; class DIconButton; class DPasswordEdit; } }
class QLabel;
class QPushButton;
class QSlider;
class QSpinBox;
class QTextEdit;

// Barra de fuerza de 5 segmentos coloreados (rojo -> verde)
class BarraFuerza : public QWidget
{
    Q_OBJECT
public:
    explicit BarraFuerza(QWidget *parent = nullptr);
    void setNivel(int nivel);  // 0..5 segmentos encendidos
    int nivel() const { return m_nivel; }
    QSize sizeHint() const override { return QSize(200, 8); }

protected:
    void paintEvent(QPaintEvent *) override;

private:
    int m_nivel = 0;
};

class Widget : public QWidget
{
    Q_OBJECT
    friend class TestWidget;

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget() override;

    enum Modo { ModoClave = 0, ModoFrase = 1, ModoComprobar = 2 };

    OpcionesClave opciones() const;
    void setOpciones(const OpcionesClave &o);
    OpcionesFrase opcionesFrase() const;
    void setOpcionesFrase(const OpcionesFrase &o);
    Modo modo() const { return m_modo; }
    void setModo(Modo m);
    // Muestra la contrasena generada o la tapa con puntos
    bool oculta() const { return m_oculta; }
    void setOculta(bool oculta);
    QString clave() const { return m_clave; }

    // Segundos que la contrasena copiada permanece en el portapapeles
    static constexpr int SEGUNDOS_PORTAPAPELES = 30;

public slots:
    void regenerar();
    void copiar();

private:
    void actualizarControles();
    void actualizarFuerza();
    void mostrarFuerza(double bits);
    void mostrar();
    void analizarEntrada();
    void borrarPortapapeles();

    Dtk::Widget::DButtonBox *m_selector;
    Dtk::Widget::DButtonBoxButton *m_btnClave, *m_btnFrase, *m_btnComprobar;
    QStackedWidget *m_superior;  // contrasena generada / campo para comprobar
    Dtk::Widget::DIconButton *m_ojo;
    Dtk::Widget::DPasswordEdit *m_entrada;
    QLabel *m_avisos;
    QStackedWidget *m_paginas;
    QTextEdit *m_campo;
    BarraFuerza *m_barra;
    QLabel *m_fuerza;
    QLabel *m_tiempo;
    QSlider *m_slider;
    QSpinBox *m_spin;
    QCheckBox *m_minusculas, *m_mayusculas, *m_numeros, *m_simbolos;
    QCheckBox *m_enie, *m_sinAmbiguos, *m_unoDeCada;
    QSlider *m_sliderPalabras;
    QSpinBox *m_spinPalabras;
    QComboBox *m_separador;
    QCheckBox *m_fraseMayusculas, *m_fraseNumero;
    QPushButton *m_generar, *m_copiar;
    Modo m_modo = ModoClave;
    bool m_oculta = false;

    QString m_clave;
    QString m_copiada;  // lo que pusimos en el portapapeles (para borrarlo)
    QTimer m_borrado;
    bool m_cargando = false;
};

#endif // WIDGET_H
