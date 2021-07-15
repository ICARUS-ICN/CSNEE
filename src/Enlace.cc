#if (defined modeMM1) && !(defined modeMM)
#define modeMM
#endif

#include "Enlace.hh"
#include <stdlib.h>

unsigned Enlace::reserved;

Enlaces::Enlaces(char *inicio)
{
  char *end = inicio, *end2 = NULL;
  unsigned i;

  for (i = 0; end2 != end; i++)
  {
    end2 = end;
    strtol(end2, &end, 0);
  }
  num_enlaces = i - 1;
  enlaces = new Enlace[num_enlaces];

  end = inicio;
  for (i = 0; i < num_enlaces; i++)
    enlaces[i] = Enlace(strtol(end, &end, 0));
}

Ruta::Ruta(char *inicio, Enlaces &grupo)
{
  char *end, *end2;
  unsigned i;

  for (end = inicio; *end; end++)
    if (*end == ',')
      *end = ' ';

  for (i = 0, end = inicio, end2 = NULL; end2 != end; i++)
  {
    end2 = end;
    strtol(end2, &end, 0);
  }
  num_enlaces = i - 1;
  e = new Enlace *[num_enlaces];

  end = inicio;
  for (i = 0; i < num_enlaces; i++)
    e[i] = &grupo[strtol(end, &end, 0)];
#ifdef modeMM
  conns = 0;
#endif
}
