#include "generador.h"

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

} // namespace Generador
