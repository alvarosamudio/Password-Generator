#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include "generador.h"

class QCheckBox;
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

    OpcionesClave opciones() const;
    void setOpciones(const OpcionesClave &o);
    QString clave() const { return m_clave; }

    // Segundos que la contrasena copiada permanece en el portapapeles
    static constexpr int SEGUNDOS_PORTAPAPELES = 30;

public slots:
    void regenerar();
    void copiar();

private:
    void actualizarControles();
    void actualizarFuerza();
    void borrarPortapapeles();

    QTextEdit *m_campo;
    BarraFuerza *m_barra;
    QLabel *m_fuerza;
    QSlider *m_slider;
    QSpinBox *m_spin;
    QCheckBox *m_minusculas, *m_mayusculas, *m_numeros, *m_simbolos;
    QCheckBox *m_enie, *m_sinAmbiguos, *m_unoDeCada;
    QPushButton *m_generar, *m_copiar;

    QString m_clave;
    QString m_copiada;  // lo que pusimos en el portapapeles (para borrarlo)
    QTimer m_borrado;
    bool m_cargando = false;
};

#endif // WIDGET_H
