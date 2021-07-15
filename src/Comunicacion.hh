#ifndef _Comunicacion_h
#define _Comunicacion_h 1

#if (defined modeMM1) && !(defined modeMM)
#define modeMM
#endif

#include "Enlace.hh"
#include <stddef.h>

enum enumTipo
{
  tipo_llegada,
  tipo_salida,
  tipo_reintento
};
typedef enumTipo Tipo;

#ifndef modeMM
class Trafico;

class Comunicacion
{
  double tiempo_ejecucion;
  Tipo tipo;
#ifdef modeGG
  Ruta *ruta_reservada;
#endif
  Trafico *trafico;

public:
#ifdef modeGG
  Comunicacion(double x = 0, Tipo t = tipo_llegada, Trafico *p = NULL, Ruta *r = NULL)
#else
  Comunicacion(double x = 0, Tipo t = tipo_llegada, Trafico *p = NULL)
#endif
  {
    tiempo_ejecucion = x;
    tipo = t;
    trafico = p;
#ifdef modeGG
    ruta_reservada = r;
#endif
  }

  void ejecuta();

  double ejecucion()
  {
    return tiempo_ejecucion;
  }
#ifdef modeGG
  Ruta *ruta()
  {
    return ruta_reservada;
  }
#endif
  friend bool operator==(const Comunicacion &c1, const Comunicacion &c2);

  // Mayor prioridad el de menor tiempo
  friend bool operator>(const Comunicacion &c1, const Comunicacion &c2);
  // Menor prioridad el de mayor tiempo
  friend bool operator<(const Comunicacion &c1, const Comunicacion &c2);
};

inline bool operator==(const Comunicacion &c1, const Comunicacion &c2)
{
  return c1.tiempo_ejecucion == c2.tiempo_ejecucion;
}

inline bool operator>(const Comunicacion &c1, const Comunicacion &c2)
{
  return c1.tiempo_ejecucion < c2.tiempo_ejecucion;
}

inline bool operator<(const Comunicacion &c1, const Comunicacion &c2)
{
  return c1.tiempo_ejecucion > c2.tiempo_ejecucion;
}
#endif
#endif
