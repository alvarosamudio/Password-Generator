#include "generador.h"

#include <QCoreApplication>
#include <QFile>
#include <QRandomGenerator>
#include <QSet>
#include <algorithm>
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
    // Media: la mitad de las combinaciones. Se trabaja en log10 para no
    // desbordar con entropias altas.
    const double log10seg = (bits - 1) * std::log10(2.0) - 10;
    if (log10seg < 0) return QCoreApplication::translate("Generador", "al instante");
    const double seg = std::pow(10.0, qMin(log10seg, 300.0));
    struct Unidad { double seg; const char *uno; const char *varios; };
    static const Unidad unidades[] = {
        {31557600.0 * 100, QT_TRANSLATE_NOOP("Generador", "1 siglo"), QT_TRANSLATE_NOOP("Generador", "%1 siglos")},
        {31557600.0, QT_TRANSLATE_NOOP("Generador", "1 año"), QT_TRANSLATE_NOOP("Generador", "%1 años")},
        {86400.0, QT_TRANSLATE_NOOP("Generador", "1 día"), QT_TRANSLATE_NOOP("Generador", "%1 días")},
        {3600.0, QT_TRANSLATE_NOOP("Generador", "1 hora"), QT_TRANSLATE_NOOP("Generador", "%1 horas")},
        {60.0, QT_TRANSLATE_NOOP("Generador", "1 minuto"), QT_TRANSLATE_NOOP("Generador", "%1 minutos")},
        {1.0, QT_TRANSLATE_NOOP("Generador", "1 segundo"), QT_TRANSLATE_NOOP("Generador", "%1 segundos")},
    };
    const double anios = seg / 31557600.0;
    if (anios >= 1.4e10) return QCoreApplication::translate("Generador", "más que la edad del universo");
    if (anios >= 1e9) return QCoreApplication::translate("Generador", "%1 mil millones de años").arg(qRound(anios / 1e9));
    if (anios >= 1e6) return QCoreApplication::translate("Generador", "%1 millones de años").arg(qRound(anios / 1e6));
    if (anios >= 1e4) return QCoreApplication::translate("Generador", "%1 mil años").arg(qRound(anios / 1e3));
    for (const Unidad &u : unidades) {
        if (seg >= u.seg) {
            const long long v = qRound64(seg / u.seg);
            return v == 1 ? QCoreApplication::translate("Generador", u.uno)
                          : QCoreApplication::translate("Generador", u.varios).arg(v);
        }
    }
    return QCoreApplication::translate("Generador", "al instante");
}

// ------------------------------------------------------------------ Analisis

namespace {

// Contrasenas y palabras muy usadas (en minusculas). Basada en las listas
// publicas de contrasenas filtradas, mas las habituales en espanol.
const char *const kComunes[] = {
    "123456", "password", "12345678", "qwerty", "123456789", "12345", "1234", "111111",
    "1234567", "dragon", "123123", "baseball", "abc123", "football", "monkey", "letmein",
    "696969", "shadow", "master", "666666", "qwertyuiop", "123321", "mustang", "1234567890",
    "michael", "654321", "superman", "1qaz2wsx", "7777777", "121212", "000000", "qazwsx",
    "123qwe", "killer", "trustno1", "jordan", "jennifer", "zxcvbnm", "asdfgh", "hunter",
    "buster", "soccer", "harley", "batman", "andrew", "tigger", "sunshine", "iloveyou",
    "charlie", "robert", "thomas", "hockey", "ranger", "daniel", "starwars", "112233",
    "george", "computer", "michelle", "jessica", "pepper", "zxcvbn", "555555", "11111111",
    "131313", "freedom", "777777", "pass", "maggie", "159753", "aaaaaa", "ginger",
    "princess", "joshua", "cheese", "amanda", "summer", "love", "ashley", "nicole",
    "chelsea", "matthew", "access", "yankees", "987654321", "dallas", "austin", "thunder",
    "taylor", "matrix", "admin", "welcome", "login", "hello", "secret", "qwerty123",
    "contrasena", "clave", "hola", "tequiero", "teamo", "amor", "barcelona", "realmadrid",
    "bocajuniors", "america", "mexico", "argentina", "colombia", "espana", "peru", "chile",
    "venezuela", "futbol", "princesa", "mariposa", "estrella", "dios", "jesus", "maria",
    "jose", "juan", "carlos", "sebastian", "alejandro", "daniela", "andrea", "linux",
    "deepin", "root", "toor", "usuario", "prueba", "test", "guest", "invitado", "admin123",
    "password1", "passw0rd", "secreto", "familia", "angel", "tesoro", "corazon", "bonita",
};

QString quitarTildes(const QString &s)
{
    QString r;
    for (const QChar c : s.normalized(QString::NormalizationForm_D))
        if (c.category() != QChar::Mark_NonSpacing) r += c;
    return r;
}

// Minusculas, sin tildes y deshaciendo las sustituciones tipicas (p4ssw0rd)
QString normalizar(const QString &s)
{
    QString r = quitarTildes(s.toLower());
    for (QChar &c : r) {
        switch (c.unicode()) {
        case '@': case '4': c = QLatin1Char('a'); break;
        case '3': c = QLatin1Char('e'); break;
        case '1': case '!': c = QLatin1Char('i'); break;
        case '0': c = QLatin1Char('o'); break;
        case '$': case '5': c = QLatin1Char('s'); break;
        case '7': c = QLatin1Char('t'); break;
        default: break;
        }
    }
    return r;
}

const QSet<QString> &comunes()
{
    static const QSet<QString> set = [] {
        QSet<QString> s;
        for (const char *c : kComunes) s.insert(QString::fromLatin1(c));
        return s;
    }();
    return set;
}

// Palabras conocidas: las comunes mas la lista de la frase de contrasena
const QSet<QString> &diccionario()
{
    static const QSet<QString> set = [] {
        QSet<QString> s;
        for (const QString &w : comunes()) {
            bool letras = true;
            for (const QChar c : w) letras = letras && c.isLetter();
            if (letras && w.size() >= 4) s.insert(w);  // "aaaaaa" o "1234" no son palabras
        }
        for (const QString &w : Generador::listaPalabras()) s.insert(w);
        return s;
    }();
    return set;
}

const char *const kFilasTeclado[] = {"qwertyuiop", "asdfghjkl", "zxcvbnm", "1234567890",
                                     "qazwsxedc", "poiuytrewq"};

} // namespace

Analisis analizar(const QString &clave)
{
    Analisis a;
    const int n = clave.size();
    if (n == 0) return a;

    // Tamano del alfabeto segun los tipos de caracter presentes
    bool minus = false, mayus = false, dig = false, otro = false;
    for (const QChar c : clave) {
        if (c.isDigit()) dig = true;
        else if (c.isLower()) minus = true;
        else if (c.isUpper()) mayus = true;
        else otro = true;
    }
    const int pool = (minus ? 26 : 0) + (mayus ? 26 : 0) + (dig ? 10 : 0) + (otro ? 33 : 0);
    const double porCaracter = std::log2(double(qMax(pool, 2)));
    const double maximo = n * porCaracter;

    // 1) Contrasena muy comun, tal cual o con numeros/simbolos al final
    QString nucleo = clave;
    int cola = 0;
    while (!nucleo.isEmpty() && !nucleo.back().isLetter()) { nucleo.chop(1); ++cola; }
    const QString completa = quitarTildes(clave.toLower());
    if (comunes().contains(completa) || comunes().contains(normalizar(clave))
        || (nucleo.size() >= 3 && (comunes().contains(quitarTildes(nucleo.toLower()))
                                   || comunes().contains(normalizar(nucleo))))) {
        a.bits = qMin(maximo, std::log2(double(comunes().size())) + cola * std::log2(10.0) + 1);
        a.avisos << QCoreApplication::translate("Generador", "Es una de las contraseñas más usadas (o una variante): es lo primero que se prueba.");
        return a;
    }

    // 2) Se recorre por fragmentos: cada patron cuenta como un solo elemento
    const QString norm = normalizar(clave);
    const QString minusculas = clave.toLower();
    const double bitsPalabra = std::log2(double(diccionario().size()));
    bool vPalabra = false, vRepite = false, vSecuencia = false, vTeclado = false, vAnio = false;
    double bits = 0;
    QSet<QChar> vistos;
    int i = 0;
    while (i < n) {
        // Mismo caracter repetido (aaa, 1111)
        int j = i + 1;
        while (j < n && clave.at(j) == clave.at(i)) ++j;
        if (j - i >= 3) {
            bits += porCaracter + std::log2(double(j - i));
            vRepite = true;
            i = j;
            continue;
        }
        // Ano (1900-2099)
        if (i + 4 <= n) {
            const QString cuatro = clave.mid(i, 4);
            bool ok = false;
            const int anio = cuatro.toInt(&ok);
            if (ok && anio >= 1900 && anio <= 2099) {
                bits += std::log2(200.0);
                vAnio = true;
                i += 4;
                continue;
            }
        }
        // Fila del teclado (4 o mas teclas seguidas, en cualquier sentido)
        int largo = 0;
        for (const char *fila : kFilasTeclado) {
            const QString f = QString::fromLatin1(fila);
            QString inv = f;
            std::reverse(inv.begin(), inv.end());
            for (int l = qMin(n - i, int(f.size())); l >= 4 && l > largo; --l) {
                const QString trozo = minusculas.mid(i, l);
                if (f.contains(trozo) || inv.contains(trozo)) { largo = l; break; }
            }
        }
        if (largo) {
            bits += std::log2(40.0) + std::log2(double(largo));
            vTeclado = true;
            i += largo;
            continue;
        }
        // Palabra del diccionario (la mas larga posible, desde 4 letras)
        largo = 0;
        for (int l = qMin(n - i, 14); l >= 4; --l)
            if (diccionario().contains(norm.mid(i, l))) { largo = l; break; }
        if (largo) {
            bits += bitsPalabra + 1;  // +1: mayusculas/sustituciones
            vPalabra = true;
            i += largo;
            continue;
        }
        // Secuencia ascendente o descendente (abc, 123, 987)
        j = i + 1;
        const int paso = (i + 1 < n) ? clave.at(i + 1).unicode() - clave.at(i).unicode() : 0;
        if (paso == 1 || paso == -1) {
            while (j < n && clave.at(j).unicode() - clave.at(j - 1).unicode() == paso) ++j;
            if (j - i >= 3) {
                bits += porCaracter + std::log2(double(j - i)) + 1;
                vSecuencia = true;
                i = j;
                continue;
            }
        }
        // Un simbolo que ya ha salido (el separador de una frase) aporta poco
        const QChar c = clave.at(i);
        if (!c.isLetterOrNumber() && vistos.contains(c))
            bits += 1;
        else
            bits += porCaracter;
        vistos.insert(c);
        ++i;
    }
    a.bits = qMin(bits, maximo);

    if (n < 8)
        a.avisos << QCoreApplication::translate("Generador", "Es demasiado corta: usa al menos 12 caracteres.");
    else if (n < 12)
        a.avisos << QCoreApplication::translate("Generador", "Es algo corta: mejor 12 caracteres o más.");
    if (int(minus) + int(mayus) + int(dig) + int(otro) == 1 && !vPalabra)
        a.avisos << QCoreApplication::translate("Generador", "Usa un solo tipo de carácter.");
    // En una frase larga las palabras son la idea; solo se avisa si debilitan
    if (vPalabra && a.bits < 60)
        a.avisos << QCoreApplication::translate("Generador", "Contiene palabras del diccionario; cambiar letras por números (p4ssw0rd) apenas ayuda.");
    if (vAnio)
        a.avisos << QCoreApplication::translate("Generador", "Contiene un año: las fechas son fáciles de adivinar.");
    if (vTeclado)
        a.avisos << QCoreApplication::translate("Generador", "Contiene teclas seguidas del teclado (qwerty, asdf…).");
    if (vSecuencia)
        a.avisos << QCoreApplication::translate("Generador", "Contiene secuencias (abc, 123…).");
    if (vRepite)
        a.avisos << QCoreApplication::translate("Generador", "Repite el mismo carácter varias veces seguidas.");
    return a;
}

} // namespace Generador
