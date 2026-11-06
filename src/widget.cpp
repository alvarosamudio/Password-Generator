#include "widget.h"

#include <DButtonBox>
#include <DIconButton>
#include <DPasswordEdit>
#include <DStyle>
#include <DMessageManager>

#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QComboBox>
#include <QFontDatabase>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QShortcut>
#include <QSlider>
#include <QSpinBox>
#include <QStackedWidget>
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

namespace {
// Deslizador + casilla numerica sincronizados
QHBoxLayout *filaNumero(QWidget *parent, const QString &texto, QSlider *&slider, QSpinBox *&spin,
                        int min, int max)
{
    auto *lbl = new QLabel(texto, parent);
    slider = new QSlider(Qt::Horizontal, parent);
    slider->setRange(min, max);
    spin = new QSpinBox(parent);
    spin->setRange(min, max);
    spin->setAccessibleName(texto);
    lbl->setBuddy(spin);
    QObject::connect(slider, &QSlider::valueChanged, spin, &QSpinBox::setValue);
    QObject::connect(spin, qOverload<int>(&QSpinBox::valueChanged), slider, &QSlider::setValue);
    auto *fila = new QHBoxLayout;
    fila->addWidget(lbl);
    fila->addSpacing(8);
    fila->addWidget(slider, 1);
    fila->addSpacing(8);
    fila->addWidget(spin);
    return fila;
}
} // namespace

Widget::Widget(QWidget *parent) : QWidget(parent)
{
    // Selector de modo
    m_selector = new DButtonBox(this);
    m_btnClave = new DButtonBoxButton(tr("Contraseña"), m_selector);
    m_btnFrase = new DButtonBoxButton(tr("Frase de contraseña"), m_selector);
    m_btnFrase->setToolTip(tr("Varias palabras al azar: más fácil de recordar y de teclear"));
    m_btnComprobar = new DButtonBoxButton(tr("Comprobar"), m_selector);
    m_btnComprobar->setToolTip(tr("Analiza la fuerza de una contraseña que ya tienes"));
    m_selector->setButtonList({m_btnClave, m_btnFrase, m_btnComprobar}, true);
    m_btnClave->setChecked(true);

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

    m_ojo = new DIconButton(DStyle::SP_ShowPassword, this);
    m_ojo->setCheckable(true);
    m_ojo->setFixedSize(40, 40);
    m_ojo->setIconSize(QSize(20, 20));
    m_ojo->setToolTip(tr("Ocultar la contraseña"));
    m_ojo->setAccessibleName(tr("Mostrar u ocultar la contraseña"));

    auto *filaCampo = new QWidget(this);
    auto *hCampo = new QHBoxLayout(filaCampo);
    hCampo->setContentsMargins(0, 0, 0, 0);
    hCampo->setSpacing(6);
    hCampo->addWidget(m_campo, 1);
    hCampo->addWidget(m_ojo, 0, Qt::AlignTop);

    // Comprobar: el texto nunca se guarda ni sale del equipo
    auto *filaEntrada = new QWidget(this);
    m_entrada = new DPasswordEdit(filaEntrada);
    m_entrada->setPlaceholderText(tr("Escribe o pega una contraseña"));
    m_entrada->setAccessibleName(tr("Contraseña a comprobar"));
    QFont fEntrada = m_entrada->font();
    fEntrada.setPointSizeF(fEntrada.pointSizeF() + 3);
    m_entrada->setFont(fEntrada);
    auto *privacidad = new QLabel(tr("Se analiza en este equipo: no se guarda ni se envía a ningún sitio."),
                                  filaEntrada);
    privacidad->setWordWrap(true);
    auto *vEntrada = new QVBoxLayout(filaEntrada);
    vEntrada->setContentsMargins(0, 8, 0, 0);
    vEntrada->addWidget(m_entrada);
    vEntrada->addWidget(privacidad);
    vEntrada->addStretch(1);

    m_superior = new QStackedWidget(this);
    m_superior->addWidget(filaCampo);
    m_superior->addWidget(filaEntrada);
    m_superior->setFixedHeight(m_campo->height());

    m_barra = new BarraFuerza(this);
    m_fuerza = new QLabel(this);
    m_fuerza->setMinimumWidth(150);
    m_fuerza->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_tiempo = new QLabel(this);
    m_tiempo->setWordWrap(true);
    QPalette pal = m_tiempo->palette();
    QColor gris = pal.color(QPalette::WindowText);
    gris.setAlphaF(0.65);
    pal.setColor(QPalette::WindowText, gris);
    m_tiempo->setPalette(pal);
    privacidad->setPalette(pal);

    // ---- Pagina "Contrasena"
    auto *pagClave = new QWidget(this);
    QHBoxLayout *longitud = filaNumero(pagClave, tr("Longitud"), m_slider, m_spin,
                                       Generador::LONGITUD_MIN, Generador::LONGITUD_MAX);
    m_minusculas = new QCheckBox(tr("Minúsculas (a-z)"), pagClave);
    m_mayusculas = new QCheckBox(tr("Mayúsculas (A-Z)"), pagClave);
    m_numeros = new QCheckBox(tr("Números (0-9)"), pagClave);
    m_simbolos = new QCheckBox(tr("Símbolos (!#$%&…)"), pagClave);
    m_enie = new QCheckBox(tr("Incluir ñ y Ñ"), pagClave);
    m_sinAmbiguos = new QCheckBox(tr("Evitar caracteres confusos (I l 1 O 0 o)"), pagClave);
    m_unoDeCada = new QCheckBox(tr("Al menos uno de cada tipo"), pagClave);
    m_enie->setToolTip(tr("Muchas webs y teclados extranjeros no aceptan la ñ"));

    auto *tipos = new QGridLayout;
    tipos->setHorizontalSpacing(24);
    tipos->addWidget(m_minusculas, 0, 0);
    tipos->addWidget(m_mayusculas, 0, 1);
    tipos->addWidget(m_numeros, 1, 0);
    tipos->addWidget(m_simbolos, 1, 1);
    tipos->addWidget(m_enie, 2, 0);
    tipos->addWidget(m_unoDeCada, 2, 1);
    tipos->addWidget(m_sinAmbiguos, 3, 0, 1, 2);
    auto *vClave = new QVBoxLayout(pagClave);
    vClave->setContentsMargins(0, 0, 0, 0);
    vClave->setSpacing(12);
    vClave->addLayout(longitud);
    vClave->addLayout(tipos);
    vClave->addStretch(1);

    // ---- Pagina "Frase de contrasena"
    auto *pagFrase = new QWidget(this);
    QHBoxLayout *palabras = filaNumero(pagFrase, tr("Palabras"), m_sliderPalabras, m_spinPalabras,
                                       Generador::PALABRAS_MIN, Generador::PALABRAS_MAX);
    m_separador = new QComboBox(pagFrase);
    m_separador->addItem(tr("Guion  ( - )"), QStringLiteral("-"));
    m_separador->addItem(tr("Punto  ( . )"), QStringLiteral("."));
    m_separador->addItem(tr("Guion bajo  ( _ )"), QStringLiteral("_"));
    m_separador->addItem(tr("Espacio"), QStringLiteral(" "));
    m_separador->addItem(tr("Ninguno"), QString());
    auto *lblSep = new QLabel(tr("Separador"), pagFrase);
    lblSep->setBuddy(m_separador);
    m_fraseMayusculas = new QCheckBox(tr("Iniciales en mayúscula"), pagFrase);
    m_fraseNumero = new QCheckBox(tr("Añadir un número"), pagFrase);
    m_fraseNumero->setToolTip(tr("Muchas webs exigen al menos un número"));
    auto *filaSep = new QHBoxLayout;
    filaSep->addWidget(lblSep);
    filaSep->addSpacing(8);
    filaSep->addWidget(m_separador);
    filaSep->addStretch(1);
    auto *opcFrase = new QHBoxLayout;
    opcFrase->setSpacing(24);
    opcFrase->addWidget(m_fraseMayusculas);
    opcFrase->addWidget(m_fraseNumero);
    opcFrase->addStretch(1);
    auto *vFrase = new QVBoxLayout(pagFrase);
    vFrase->setContentsMargins(0, 0, 0, 0);
    vFrase->setSpacing(12);
    vFrase->addLayout(palabras);
    vFrase->addLayout(filaSep);
    vFrase->addLayout(opcFrase);
    vFrase->addStretch(1);

    // ---- Pagina "Comprobar": lista de problemas encontrados
    auto *pagComprobar = new QWidget(this);
    m_avisos = new QLabel(pagComprobar);
    m_avisos->setWordWrap(true);
    m_avisos->setTextFormat(Qt::RichText);
    m_avisos->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    auto *vComprobar = new QVBoxLayout(pagComprobar);
    vComprobar->setContentsMargins(0, 0, 0, 0);
    vComprobar->addWidget(m_avisos);
    vComprobar->addStretch(1);

    m_paginas = new QStackedWidget(this);
    m_paginas->addWidget(pagClave);
    m_paginas->addWidget(pagFrase);
    m_paginas->addWidget(pagComprobar);

    m_generar = new QPushButton(tr("Generar otra"), this);
    m_copiar = new QPushButton(tr("Copiar"), this);
    m_generar->setToolTip(tr("Generar otra (F5)"));
    m_copiar->setToolTip(tr("Copiar al portapapeles (Ctrl+C). Se borra a los %1 s.")
                             .arg(SEGUNDOS_PORTAPAPELES));

    // Distribucion
    auto *selector = new QHBoxLayout;
    selector->addStretch(1);
    selector->addWidget(m_selector);
    selector->addStretch(1);

    auto *fuerza = new QHBoxLayout;
    fuerza->addWidget(m_barra, 1);
    fuerza->addSpacing(12);
    fuerza->addWidget(m_fuerza);

    auto *botones = new QHBoxLayout;
    botones->addWidget(m_generar, 1);
    botones->addWidget(m_copiar, 1);

    auto *v = new QVBoxLayout(this);
    v->setContentsMargins(20, 6, 20, 20);
    v->setSpacing(10);
    v->addLayout(selector);
    v->addWidget(m_superior);
    v->addLayout(fuerza);
    v->addWidget(m_tiempo);
    v->addSpacing(6);
    v->addWidget(m_paginas, 1);
    v->addLayout(botones);

    // Cualquier cambio genera una contrasena nueva al momento
    connect(m_spin, qOverload<int>(&QSpinBox::valueChanged), this, &Widget::regenerar);
    connect(m_spinPalabras, qOverload<int>(&QSpinBox::valueChanged), this, &Widget::regenerar);
    connect(m_separador, qOverload<int>(&QComboBox::currentIndexChanged), this, &Widget::regenerar);
    for (QCheckBox *c : {m_minusculas, m_mayusculas, m_numeros, m_simbolos, m_enie,
                         m_sinAmbiguos, m_unoDeCada, m_fraseMayusculas, m_fraseNumero})
        connect(c, &QCheckBox::toggled, this, &Widget::regenerar);
    connect(m_selector, &DButtonBox::buttonClicked, this, [this](QAbstractButton *b) {
        setModo(b == m_btnFrase ? ModoFrase : b == m_btnComprobar ? ModoComprobar : ModoClave);
    });
    connect(m_ojo, &DIconButton::toggled, this, &Widget::setOculta);
    connect(m_entrada, &DLineEdit::textChanged, this, &Widget::analizarEntrada);
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

    m_cargando = true;
    setOpcionesFrase(OpcionesFrase{});
    m_cargando = false;
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
    const bool cargando = m_cargando;
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
    m_cargando = cargando;
    regenerar();
}

OpcionesFrase Widget::opcionesFrase() const
{
    OpcionesFrase o;
    o.palabras = m_spinPalabras->value();
    o.separador = m_separador->currentData().toString();
    o.mayusculas = m_fraseMayusculas->isChecked();
    o.numero = m_fraseNumero->isChecked();
    return o;
}

void Widget::setOpcionesFrase(const OpcionesFrase &o)
{
    const bool cargando = m_cargando;
    m_cargando = true;
    m_spinPalabras->setValue(qBound(Generador::PALABRAS_MIN, o.palabras, Generador::PALABRAS_MAX));
    const int i = m_separador->findData(o.separador);
    m_separador->setCurrentIndex(i >= 0 ? i : 0);
    m_fraseMayusculas->setChecked(o.mayusculas);
    m_fraseNumero->setChecked(o.numero);
    m_cargando = cargando;
    regenerar();
}

void Widget::setModo(Modo m)
{
    m_modo = m;
    (m == ModoFrase ? m_btnFrase : m == ModoComprobar ? m_btnComprobar : m_btnClave)->setChecked(true);
    m_paginas->setCurrentIndex(m);
    const bool comprobar = m == ModoComprobar;
    m_superior->setCurrentIndex(comprobar ? 1 : 0);
    m_generar->setVisible(!comprobar);
    m_copiar->setVisible(!comprobar);
    if (comprobar) {
        m_entrada->setFocus();
        analizarEntrada();
    } else {
        m_entrada->clear();  // no dejar la contrasena escrita en memoria
        regenerar();
    }
}

void Widget::setOculta(bool oculta)
{
    m_oculta = oculta;
    if (m_ojo->isChecked() != oculta) m_ojo->setChecked(oculta);
    m_ojo->setIcon(oculta ? DStyle::SP_HidePassword : DStyle::SP_ShowPassword);
    m_ojo->setToolTip(oculta ? tr("Mostrar la contraseña") : tr("Ocultar la contraseña"));
    mostrar();
}

void Widget::analizarEntrada()
{
    if (m_modo != ModoComprobar) return;
    const QString texto = m_entrada->text();
    if (texto.isEmpty()) {
        m_barra->setNivel(0);
        m_fuerza->clear();
        m_tiempo->setText(tr("Escribe una contraseña para ver su fuerza real y qué la debilita."));
        m_avisos->clear();
        return;
    }
    const Analisis a = Generador::analizar(texto);
    mostrarFuerza(a.bits);
    if (a.avisos.isEmpty()) {
        m_avisos->setText(QStringLiteral("<p style='color:#46a758'>✔ %1</p>")
                              .arg(tr("No se han encontrado puntos débiles.").toHtmlEscaped()));
    } else {
        QString html = QStringLiteral("<ul style='margin-left:-20px'>");
        for (const QString &aviso : a.avisos)
            html += QStringLiteral("<li style='margin-bottom:4px'>%1</li>").arg(aviso.toHtmlEscaped());
        m_avisos->setText(html + QStringLiteral("</ul>"));
    }
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
    mostrarFuerza(m_modo == ModoFrase ? Generador::entropiaFrase(opcionesFrase())
                                      : Generador::entropia(opciones()));
}

void Widget::mostrarFuerza(double bits)
{
    QString texto;
    switch (Generador::fuerza(bits)) {
    case Generador::Fuerza::MuyDebil:  m_barra->setNivel(1); texto = tr("Muy débil"); break;
    case Generador::Fuerza::Debil:     m_barra->setNivel(2); texto = tr("Débil"); break;
    case Generador::Fuerza::Aceptable: m_barra->setNivel(3); texto = tr("Aceptable"); break;
    case Generador::Fuerza::Fuerte:    m_barra->setNivel(4); texto = tr("Fuerte"); break;
    case Generador::Fuerza::MuyFuerte: m_barra->setNivel(5); texto = tr("Muy fuerte"); break;
    }
    m_fuerza->setText(tr("%1 · %2 bits").arg(texto).arg(qRound(bits)));
    m_fuerza->setToolTip(tr("Entropía estimada: hacen falta de media 2 elevado a ese número "
                            "(entre dos) intentos para adivinarla."));
    m_tiempo->setText(tr("Un equipo muy potente tardaría %1 en adivinarla.")
                          .arg(Generador::tiempoDescifrado(bits)));
    m_tiempo->setToolTip(tr("Estimación con 10 000 millones de intentos por segundo."));
}

// Colorea numeros y simbolos para distinguirlos de las letras de un vistazo
void Widget::mostrar()
{
    if (m_oculta) {
        m_campo->setPlainText(QString(m_clave.size(), QChar(0x2022)));
        m_campo->setAlignment(Qt::AlignCenter);
        static_cast<CampoClave *>(m_campo)->centrar();
        return;
    }
    QString html;
    html.reserve(m_clave.size() * 40);
    for (const QChar c : m_clave) {
        const QString e = QString(c).toHtmlEscaped();
        if (c.isDigit())
            html += QStringLiteral("<span style='color:#3d8bfd'>%1</span>").arg(e);
        else if (!c.isLetter())
            html += QStringLiteral("<span style='color:#f08c00'>%1</span>").arg(e);
        else
            html += e;
    }
    // pre-wrap conserva los espacios tal cual (con &nbsp; se copiaria un
    // espacio distinto al seleccionar a mano)
    m_campo->setHtml(QStringLiteral("<span style='white-space:pre-wrap'>%1</span>").arg(html));
    m_campo->setAlignment(Qt::AlignCenter);
    static_cast<CampoClave *>(m_campo)->centrar();
}

void Widget::regenerar()
{
    if (m_cargando || m_modo == ModoComprobar) return;
    actualizarControles();
    m_clave = m_modo == ModoFrase ? Generador::generarFrase(opcionesFrase())
                                  : Generador::generar(opciones());
    mostrar();
    actualizarFuerza();
}

void Widget::copiar()
{
    if (m_clave.isEmpty() || m_modo == ModoComprobar) return;
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
