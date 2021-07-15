#ifndef _Salida_h
#define _Salida_h 1

#include "Enlace.hh"

class Salida
{
  double tiempo_ejecucion;
  Ruta *ruta_reservada;

public:
  Salida(double x = 0, Ruta *r = NULL)
  {
    tiempo_ejecucion = x;
    ruta_reservada = r;
  }

  Ruta *ruta()
  {
    return ruta_reservada;
  }

  double ejecucion()
  {
    return tiempo_ejecucion;
  }

  friend int operator==(Salida &c1, Salida &c2);
  friend int operator<=(Salida &c1, Salida &c2);
};

inline int operator==(Salida &c1, Salida &c2)
{
  return c1.tiempo_ejecucion == c2.tiempo_ejecucion;
}

inline int operator<=(Salida &c1, Salida &c2)
{
  return c1.tiempo_ejecucion <= c2.tiempo_ejecucion;
}

#endif
