#ifndef GENERADOR_H
#define GENERADOR_H

#include <QString>
#include <QStringList>

class QRandomGenerator;

struct OpcionesClave {
    int longitud = 16;
    bool minusculas = true;
    bool mayusculas = true;
    bool numeros = true;
    bool simbolos = true;
    bool enie = false;          // anade ñ / Ñ a las letras
    bool sinAmbiguos = false;   // quita caracteres que se confunden (I l 1 O 0 o |)
    bool unoDeCada = true;      // al menos un caracter de cada tipo elegido
};

// Frase de contrasena: palabras comunes al azar (faciles de recordar y teclear)
struct OpcionesFrase {
    int palabras = 6;
    QString separador = QStringLiteral("-");
    bool mayusculas = true;  // inicial de cada palabra en mayuscula
    bool numero = true;      // un digito al final de una palabra al azar
};

// Resultado de analizar una contrasena escrita por el usuario
struct Analisis {
    double bits = 0;      // entropia estimada teniendo en cuenta los patrones
    QStringList avisos;   // problemas encontrados, ya traducidos
};

// Logica pura del generador (sin interfaz), para poder probarla.
namespace Generador {

static constexpr int LONGITUD_MIN = 4;
static constexpr int LONGITUD_MAX = 128;

enum class Fuerza { MuyDebil, Debil, Aceptable, Fuerte, MuyFuerte };

// Caracteres de cada tipo elegido, ya filtrados
QStringList grupos(const OpcionesClave &o);
// Todos los caracteres posibles, sin repetir
QString juego(const OpcionesClave &o);
// Contrasena aleatoria; vacia si no hay ningun tipo elegido. Por defecto usa
// el generador criptografico del sistema operativo.
QString generar(const OpcionesClave &o, QRandomGenerator *rng = nullptr);
// Entropia aproximada en bits: longitud * log2(tamano del juego)
double entropia(const OpcionesClave &o);
Fuerza fuerza(double bits);

static constexpr int PALABRAS_MIN = 3;
static constexpr int PALABRAS_MAX = 12;

// Lista de palabras (recurso :/palabras.txt), cargada una sola vez
const QStringList &listaPalabras();
QString generarFrase(const OpcionesFrase &o, QRandomGenerator *rng = nullptr);
// log2(tamano de la lista) por palabra, mas lo que aporte el numero
double entropiaFrase(const OpcionesFrase &o);

// Tiempo medio para adivinarla a 10 000 millones de intentos por segundo
// (un equipo potente contra un hash rapido), en palabras: "3 siglos"
QString tiempoDescifrado(double bits);

// Estima la fuerza real de una contrasena existente: detecta contrasenas muy
// comunes, palabras del diccionario (tambien con sustituciones tipo p4ssw0rd),
// repeticiones, secuencias, filas del teclado y anos. Todo en local.
Analisis analizar(const QString &clave);

} // namespace Generador

#endif
