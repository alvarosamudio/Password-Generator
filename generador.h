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

} // namespace Generador

#endif
