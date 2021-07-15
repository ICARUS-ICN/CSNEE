#ifndef _ENLACE_h
#define _ENLACE_h 1

#if (defined MM1) && !(defined MM)
#define MM
#endif

#include <iostream>
#include <stdlib.h>
using namespace std;

class Enlace
{
  unsigned libres;
  static unsigned reserved;

public:
  static void init_reserved(unsigned number_reserved)
  {
    reserved = number_reserved;
  }

  Enlace(unsigned circuitos = 0)
  {
    libres = circuitos;
  }

  int reserva(unsigned overload = 0)
  {
    if (overload ? (libres <= reserved) : !libres)
      return 0;

    libres--;
    return 1;
  }

  void libera(void)
  {
    libres++;
  }

  unsigned disponible(void)
  {
    return libres;
  }
};

class Enlaces
{
  Enlace *enlaces;
  unsigned num_enlaces;

public:
  Enlaces(char *inicio);

  Enlace &operator[](unsigned num)
  {
    if (num >= num_enlaces)
    {
      cerr << "Acceso a posicion " << num << " a objeto Enlaces fuera del limite "
           << num_enlaces - 1 << endl;
      exit(1);
    }
    return enlaces[num];
  }
};

class Ruta
{
  Enlace **e;
  unsigned num_enlaces;
#ifdef modeMM
  unsigned conns;
#endif

public:
  Ruta(char *inicio, Enlaces &grupo);

  int reserva(unsigned overload = 0)
  {
    for (Enlace **p = e, **l = e + num_enlaces; p != l; p++)
      if (!(*p)->reserva(overload))
      {
        if (p != e)
          do
            (*--p)->libera();
          while (p != e);
        return (0);
      }
#ifdef modeMM
    conns++;
#endif
    return (1);
  }

#ifdef modeMM
  void libera(unsigned *num_conn)
  {
    if (*num_conn <= conns)
    {
      for (Enlace **p = e, **l = e + num_enlaces; p != l; (*p++)->libera())
        ;
      conns--;
      *num_conn = 0;
    }
    else
      *num_conn -= conns;
  }
#else
  void libera(void)
  {
    for (Enlace **p = e, **l = e + num_enlaces; p != l; (*p++)->libera())
      ;
  }
#endif

  unsigned mejora(unsigned min_prev)
  {
    unsigned min = 0xFFFFFFFF;
    for (Enlace **p = e, **l = e + num_enlaces; p != l; p++)
    {
      unsigned m = (*p)->disponible();
      if (m < min)
      {
        if (m <= min_prev)
          return 0;
        min = m;
      }
    }
    return min;
  }
};
#endif
