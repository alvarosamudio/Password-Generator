#include "widget.h"

#include <DMessageManager>

#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QFontDatabase>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QShortcut>
#include <QSlider>
#include <QSpinBox>
#include <QTextEdit>
#include <QTextFrame>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

// ---------------------------------------------------------------- BarraFuerza

BarraFuerza::BarraFuerza(QWidget *parent) : QWidget(parent)
{
    setMinimumHeight(8);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

void BarraFuerza::setNivel(int nivel)
{
    nivel = qBound(0, nivel, 5);
    if (nivel == m_nivel) return;
    m_nivel = nivel;
    update();
}

void BarraFuerza::paintEvent(QPaintEvent *)
{
    static const QColor colores[5] = {QColor("#e5484d"), QColor("#f76b15"), QColor("#f5b400"),
                                      QColor("#46a758"), QColor("#2e8b57")};
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    const int hueco = 6;
    const qreal ancho = (width() - 4 * hueco) / 5.0;
    QColor apagado = palette().color(QPalette::WindowText);
    apagado.setAlphaF(0.12);
    for (int i = 0; i < 5; ++i) {
        p.setBrush(i < m_nivel ? colores[m_nivel - 1] : apagado);
        p.drawRoundedRect(QRectF(i * (ancho + hueco), 0, ancho, height()), 4, 4);
    }
}

// ---------------------------------------------------------------- CampoClave

namespace {
// Campo de solo lectura que centra el texto tambien en vertical
class CampoClave : public QTextEdit
{
public:
    using QTextEdit::QTextEdit;
    // Margen superior en el propio documento (con setViewportMargins quedaba
    // una franja de otro color encima del texto)
    void centrar()
    {
        QTextFrame *raiz = document()->rootFrame();
        QTextFrameFormat f = raiz->frameFormat();
        f.setTopMargin(0);
        raiz->setFrameFormat(f);
        const qreal alto = document()->size().height();
        f.setTopMargin(qMax(0.0, (viewport()->height() - alto) / 2));
        raiz->setFrameFormat(f);
    }

protected:
    void resizeEvent(QResizeEvent *e) override
    {
        QTextEdit::resizeEvent(e);
        centrar();
    }
};
} // namespace

// --------------------------------------------------------------------- Widget

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    // Contrasena: monoespaciada, grande, seleccionable y partida en cualquier
    // caracter para que quepan las largas.
    m_campo = new CampoClave(this);
    m_campo->setReadOnly(true);
    m_campo->setAcceptRichText(false);
    m_campo->setWordWrapMode(QTextOption::WrapAnywhere);
    m_campo->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_campo->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // Monoespaciada: cada caracter ocupa lo mismo y se distinguen mejor
    QFont mono(QStringLiteral("monospace"));
    mono.setStyleHint(QFont::TypeWriter);
    mono.setFixedPitch(true);
    mono.setPointSizeF(QFontDatabase::systemFont(QFontDatabase::GeneralFont).pointSizeF() + 5);
    m_campo->setFont(mono);
    m_campo->setFixedHeight(QFontMetrics(mono).lineSpacing() * 3 + 16);
    m_campo->setAccessibleName(tr("Contraseña generada"));

    m_barra = new BarraFuerza(this);
    m_fuerza = new QLabel(this);
    m_fuerza->setMinimumWidth(150);
    m_fuerza->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    auto *lblLongitud = new QLabel(tr("Longitud"), this);
    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(Generador::LONGITUD_MIN, Generador::LONGITUD_MAX);
    m_spin = new QSpinBox(this);
    m_spin->setRange(Generador::LONGITUD_MIN, Generador::LONGITUD_MAX);
    m_spin->setAccessibleName(tr("Longitud"));
    lblLongitud->setBuddy(m_spin);

    m_minusculas = new QCheckBox(tr("Minúsculas (a-z)"), this);
    m_mayusculas = new QCheckBox(tr("Mayúsculas (A-Z)"), this);
    m_numeros = new QCheckBox(tr("Números (0-9)"), this);
    m_simbolos = new QCheckBox(tr("Símbolos (!#$%&…)"), this);
    m_enie = new QCheckBox(tr("Incluir ñ y Ñ"), this);
    m_sinAmbiguos = new QCheckBox(tr("Evitar caracteres confusos (I l 1 O 0 o)"), this);
    m_unoDeCada = new QCheckBox(tr("Al menos uno de cada tipo"), this);
    m_enie->setToolTip(tr("Muchas webs y teclados extranjeros no aceptan la ñ"));

    m_generar = new QPushButton(tr("Generar otra"), this);
    m_copiar = new QPushButton(tr("Copiar"), this);
    m_generar->setToolTip(tr("Generar otra contraseña (F5)"));
    m_copiar->setToolTip(tr("Copiar al portapapeles (Ctrl+C). Se borra a los %1 s.")
                             .arg(SEGUNDOS_PORTAPAPELES));

    // Distribucion
    auto *fuerza = new QHBoxLayout;
    fuerza->addWidget(m_barra, 1);
    fuerza->addSpacing(12);
    fuerza->addWidget(m_fuerza);

    auto *longitud = new QHBoxLayout;
    longitud->addWidget(lblLongitud);
    longitud->addSpacing(8);
    longitud->addWidget(m_slider, 1);
    longitud->addSpacing(8);
    longitud->addWidget(m_spin);

    auto *tipos = new QGridLayout;
    tipos->setHorizontalSpacing(24);
    tipos->addWidget(m_minusculas, 0, 0);
    tipos->addWidget(m_mayusculas, 0, 1);
    tipos->addWidget(m_numeros, 1, 0);
    tipos->addWidget(m_simbolos, 1, 1);
    tipos->addWidget(m_enie, 2, 0);
    tipos->addWidget(m_unoDeCada, 2, 1);
    tipos->addWidget(m_sinAmbiguos, 3, 0, 1, 2);

    auto *botones = new QHBoxLayout;
    botones->addWidget(m_generar, 1);
    botones->addWidget(m_copiar, 1);

    auto *v = new QVBoxLayout(this);
    v->setContentsMargins(20, 10, 20, 20);
    v->setSpacing(12);
    v->addWidget(m_campo);
    v->addLayout(fuerza);
    v->addSpacing(4);
    v->addLayout(longitud);
    v->addLayout(tipos);
    v->addStretch(1);
    v->addLayout(botones);

    // Cualquier cambio genera una contrasena nueva al momento
    connect(m_slider, &QSlider::valueChanged, m_spin, &QSpinBox::setValue);
    connect(m_spin, qOverload<int>(&QSpinBox::valueChanged), m_slider, &QSlider::setValue);
    connect(m_spin, qOverload<int>(&QSpinBox::valueChanged), this, &Widget::regenerar);
    for (QCheckBox *c : {m_minusculas, m_mayusculas, m_numeros, m_simbolos, m_enie,
                         m_sinAmbiguos, m_unoDeCada})
        connect(c, &QCheckBox::toggled, this, &Widget::regenerar);
    connect(m_generar, &QPushButton::clicked, this, &Widget::regenerar);
    connect(m_copiar, &QPushButton::clicked, this, &Widget::copiar);

    new QShortcut(QKeySequence::Refresh, this, this, &Widget::regenerar);
    new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_R), this, this, &Widget::regenerar);
    // Ctrl+C copia la contrasena entera; si el campo tiene el foco y hay texto
    // seleccionado, el propio campo copia solo la seleccion.
    new QShortcut(QKeySequence::Copy, this, this, &Widget::copiar);

    m_borrado.setSingleShot(true);
    m_borrado.setInterval(SEGUNDOS_PORTAPAPELES * 1000);
    connect(&m_borrado, &QTimer::timeout, this, &Widget::borrarPortapapeles);

    setOpciones(OpcionesClave{});
}

Widget::~Widget()
{
    // No dejar la contrasena en el portapapeles al cerrar
    borrarPortapapeles();
}

OpcionesClave Widget::opciones() const
{
    OpcionesClave o;
    o.longitud = m_spin->value();
    o.minusculas = m_minusculas->isChecked();
    o.mayusculas = m_mayusculas->isChecked();
    o.numeros = m_numeros->isChecked();
    o.simbolos = m_simbolos->isChecked();
    o.enie = m_enie->isChecked();
    o.sinAmbiguos = m_sinAmbiguos->isChecked();
    o.unoDeCada = m_unoDeCada->isChecked();
    return o;
}

void Widget::setOpciones(const OpcionesClave &o)
{
    m_cargando = true;
    m_spin->setValue(qBound(Generador::LONGITUD_MIN, o.longitud, Generador::LONGITUD_MAX));
    m_minusculas->setChecked(o.minusculas);
    m_mayusculas->setChecked(o.mayusculas);
    m_numeros->setChecked(o.numeros);
    m_simbolos->setChecked(o.simbolos);
    // Siempre debe quedar al menos un tipo de caracter
    if (!o.minusculas && !o.mayusculas && !o.numeros && !o.simbolos)
        m_minusculas->setChecked(true);
    m_enie->setChecked(o.enie);
    m_sinAmbiguos->setChecked(o.sinAmbiguos);
    m_unoDeCada->setChecked(o.unoDeCada);
    m_cargando = false;
    regenerar();
}

void Widget::actualizarControles()
{
    // El ultimo tipo marcado no se puede desmarcar: nunca hay contrasena vacia
    const QList<QCheckBox *> tipos = {m_minusculas, m_mayusculas, m_numeros, m_simbolos};
    int marcados = 0;
    for (QCheckBox *c : tipos) marcados += c->isChecked();
    for (QCheckBox *c : tipos) c->setEnabled(!(marcados == 1 && c->isChecked()));
    m_enie->setEnabled(m_minusculas->isChecked() || m_mayusculas->isChecked());
    m_unoDeCada->setEnabled(marcados > 1);
}

void Widget::actualizarFuerza()
{
    const double bits = Generador::entropia(opciones());
    QString texto;
    switch (Generador::fuerza(bits)) {
    case Generador::Fuerza::MuyDebil:  m_barra->setNivel(1); texto = tr("Muy débil"); break;
    case Generador::Fuerza::Debil:     m_barra->setNivel(2); texto = tr("Débil"); break;
    case Generador::Fuerza::Aceptable: m_barra->setNivel(3); texto = tr("Aceptable"); break;
    case Generador::Fuerza::Fuerte:    m_barra->setNivel(4); texto = tr("Fuerte"); break;
    case Generador::Fuerza::MuyFuerte: m_barra->setNivel(5); texto = tr("Muy fuerte"); break;
    }
    m_fuerza->setText(tr("%1 · %2 bits").arg(texto).arg(qRound(bits)));
    m_fuerza->setToolTip(tr("Entropía estimada: cuántos intentos haría falta para adivinarla "
                            "(2 elevado a ese número)."));
}

void Widget::regenerar()
{
    if (m_cargando) return;
    actualizarControles();
    m_clave = Generador::generar(opciones());
    m_campo->setPlainText(m_clave);
    m_campo->setAlignment(Qt::AlignCenter);
    static_cast<CampoClave *>(m_campo)->centrar();
    actualizarFuerza();
}

void Widget::copiar()
{
    if (m_clave.isEmpty()) return;
    QApplication::clipboard()->setText(m_clave);
    m_copiada = m_clave;
    m_borrado.start();
    if (window() && window()->isVisible())
        DMessageManager::instance()->sendMessage(
            window(), QIcon::fromTheme(QStringLiteral("dialog-ok")),
            tr("Contraseña copiada. Se borrará del portapapeles en %1 segundos.")
                .arg(SEGUNDOS_PORTAPAPELES));
}

void Widget::borrarPortapapeles()
{
    m_borrado.stop();
    if (m_copiada.isEmpty()) return;
    // Solo si sigue siendo nuestra: no borrar algo que el usuario copio despues
    QClipboard *cb = QApplication::clipboard();
    if (cb->text() == m_copiada) cb->clear();
    m_copiada.clear();
}
