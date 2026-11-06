#include "generador.h"

#include <QCoreApplication>
#include <QFile>
#include <QRandomGenerator>
#include <QtMath>

namespace {
const QString kMinusculas = QStringLiteral("abcdefghijklmnopqrstuvwxyz");
const QString kMayusculas = QStringLiteral("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
const QString kNumeros = QStringLiteral("0123456789");
// Solo ASCII y sin comillas, barra invertida, acento grave ni espacio: son los
// que mas rechazan las webs y los que dan problemas al pegarlos en una consola.
const QString kSimbolos = QStringLiteral("!#$%&()*+,-./:;<=>?@[]^_{|}~");
const QString kAmbiguos = QStringLiteral("Il1O0o|");

QString filtrar(QString s, bool sinAmbiguos)
{
    if (sinAmbiguos)
        for (const QChar c : kAmbiguos) s.remove(c);
    return s;
}
} // namespace

namespace Generador {

QStringList grupos(const OpcionesClave &o)
{
    QStringList g;
    if (o.minusculas) g << filtrar(kMinusculas + (o.enie ? QStringLiteral("ñ") : QString()), o.sinAmbiguos);
    if (o.mayusculas) g << filtrar(kMayusculas + (o.enie ? QStringLiteral("Ñ") : QString()), o.sinAmbiguos);
    if (o.numeros) g << filtrar(kNumeros, o.sinAmbiguos);
    if (o.simbolos) g << filtrar(kSimbolos, o.sinAmbiguos);
    return g;
}

QString juego(const OpcionesClave &o)
{
    return grupos(o).join(QString());
}

QString generar(const OpcionesClave &o, QRandomGenerator *rng)
{
    if (!rng) rng = QRandomGenerator::system();
    const QStringList g = grupos(o);
    const QString todos = g.join(QString());
    const int n = qBound(LONGITUD_MIN, o.longitud, LONGITUD_MAX);
    if (todos.isEmpty()) return QString();

    // "Al menos uno de cada": se descartan las contrasenas que no lo cumplen y
    // se vuelve a sortear. Asi todas las validas siguen siendo igual de
    // probables (colocar a mano un caracter de cada tipo sesgaria el reparto).
    const bool exigir = o.unoDeCada && n >= g.size();
    QString clave;
    for (int intento = 0; intento < 10000; ++intento) {
        clave.clear();
        clave.reserve(n);
        for (int i = 0; i < n; ++i)
            clave.append(todos.at(int(rng->bounded(quint32(todos.size())))));
        if (!exigir) break;
        bool ok = true;
        for (const QString &grupo : g) {
            bool hay = false;
            for (const QChar c : clave)
                if (grupo.contains(c)) { hay = true; break; }
            if (!hay) { ok = false; break; }
        }
        if (ok) break;
    }
    return clave;
}

double entropia(const OpcionesClave &o)
{
    const int tam = juego(o).size();
    if (tam < 2) return 0.0;
    return qBound(LONGITUD_MIN, o.longitud, LONGITUD_MAX) * std::log2(double(tam));
}

Fuerza fuerza(double bits)
{
    if (bits < 40) return Fuerza::MuyDebil;
    if (bits < 60) return Fuerza::Debil;
    if (bits < 80) return Fuerza::Aceptable;
    if (bits < 112) return Fuerza::Fuerte;
    return Fuerza::MuyFuerte;
}

const QStringList &listaPalabras()
{
    static const QStringList lista = [] {
        QStringList l;
        QFile f(QStringLiteral(":/palabras.txt"));
        if (f.open(QIODevice::ReadOnly | QIODevice::Text))
            for (const QByteArray &linea : f.readAll().split('\n'))
                if (!linea.trimmed().isEmpty()) l << QString::fromUtf8(linea.trimmed());
        l.removeDuplicates();
        return l;
    }();
    return lista;
}

QString generarFrase(const OpcionesFrase &o, QRandomGenerator *rng)
{
    if (!rng) rng = QRandomGenerator::system();
    const QStringList &lista = listaPalabras();
    if (lista.isEmpty()) return QString();
    const int n = qBound(PALABRAS_MIN, o.palabras, PALABRAS_MAX);

    QStringList elegidas;
    for (int i = 0; i < n; ++i) {
        QString w = lista.at(int(rng->bounded(quint32(lista.size()))));
        if (o.mayusculas) w[0] = w[0].toUpper();
        elegidas << w;
    }
    if (o.numero) {
        const int cual = int(rng->bounded(quint32(n)));
        elegidas[cual] += QString::number(rng->bounded(10));
    }
    return elegidas.join(o.separador);
}

double entropiaFrase(const OpcionesFrase &o)
{
    const int tam = listaPalabras().size();
    if (tam < 2) return 0.0;
    const int n = qBound(PALABRAS_MIN, o.palabras, PALABRAS_MAX);
    double bits = n * std::log2(double(tam));
    if (o.numero) bits += std::log2(10.0 * n);  // que digito y en que palabra
    return bits;
}

QString tiempoDescifrado(double bits)
{
    auto tr = [](const char *t) { return QCoreApplication::translate("Generador", t); };
    // Media: la mitad de las combinaciones. Se trabaja en log10 para no
    // desbordar con entropias altas.
    const double log10seg = (bits - 1) * std::log10(2.0) - 10;
    if (log10seg < 0) return tr("al instante");
    const double seg = std::pow(10.0, qMin(log10seg, 300.0));
    struct Unidad { double seg; const char *uno; const char *varios; };
    static const Unidad unidades[] = {
        {31557600.0 * 100, "1 siglo", "%1 siglos"},
        {31557600.0, "1 año", "%1 años"},
        {86400.0, "1 día", "%1 días"},
        {3600.0, "1 hora", "%1 horas"},
        {60.0, "1 minuto", "%1 minutos"},
        {1.0, "1 segundo", "%1 segundos"},
    };
    const double anios = seg / 31557600.0;
    if (anios >= 1.4e10) return tr("más que la edad del universo");
    if (anios >= 1e9) return tr("%1 mil millones de años").arg(qRound(anios / 1e9));
    if (anios >= 1e6) return tr("%1 millones de años").arg(qRound(anios / 1e6));
    if (anios >= 1e4) return tr("%1 mil años").arg(qRound(anios / 1e3));
    for (const Unidad &u : unidades) {
        if (seg >= u.seg) {
            const long long v = qRound64(seg / u.seg);
            return v == 1 ? tr(u.uno) : tr(u.varios).arg(v);
        }
    }
    return tr("al instante");
}

} // namespace Generador
