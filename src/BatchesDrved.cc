#define BATCHESDRVED_CC

#include "BatchesDrved.hh"

static double fiz[] = {
    0.5, 0.0,
    0.6, 0.254093,
    0.7, 0.526886,
    0.8, 0.847686,
    0.9, 1.29649,
    0.9333, 1.52248,
    0.95, 1.67193,
    0.96, 1.78232,
    0.9667, 1.86997,
    0.975, 2.00219,
    0.98, 2.10146,
    0.9833, 2.1796,
    0.9875, 2.3016,
    0.99, 2.3929,
    0.9917, 2.46751,
    0.9938, 2.58161,
    0.9950, 2.66381};

static double tval(double p, int df)
{
  double t;
  int positive = p >= 0.5;
  p = (positive) ? 1.0 - p : p;
  if (p <= 0.0 || df <= 0)
    t = HUGE_VAL;
  else if (p == 0.5)
    t = 0.0;
  else if (df == 1)
    t = 1.0 / tan((p + p) * 1.57079633);
  else if (df == 2)
    t = sqrt(1.0 / ((p + p) * (1.0 - p)) - 2.0);
  else
  {
    double ddf = df;
    double a = sqrt(log(1.0 / (p * p)));
    double aa = a * a;
    a = a - ((2.515517 + (0.802853 * a) + (0.010328 * aa)) /
             (1.0 + (1.432788 * a) + (0.189269 * aa) +
              (0.001308 * aa * a)));
    t = ddf - 0.666666667 + 1.0 / (10.0 * ddf);
    t = sqrt(ddf * (exp(a * a * (ddf - 0.833333333) / (t * t)) - 1.0));
  }
  return (positive) ? t : -t;
}

static double t_quality(double precision)
{
  double *x;
  double delta_precision = 0.001;
  double p = 0.995;

  if (precision >= 1E30 / 3.0)
    return (1.0);

  if (precision > 2.66381 && precision < 1E30 / 3.0)
  {

    while (tval(p, 58) < precision)
    {
      if ((p + delta_precision) >= 1.0 - 1e-8)
      {
        delta_precision /= 10.0;
        if (delta_precision < 1e-6)
          break;
      }
      p += delta_precision;
    }

    if (delta_precision < 1e-6)
      precision = 0.999999;
    else
      precision =
          (delta_precision / (tval(p, 58) - tval(p - delta_precision, 58))) *
              (precision - tval(p - delta_precision, 58)) +
          p;
  }

  else
  {
    for (x = &fiz[1]; precision > *(x + 2); x += 2)
      ;

    precision = (*(x + 1) - *(x - 1)) / (*(x + 2) - *x) *
                    (precision - *x) +
                *(x - 1);
  }

  precision = 2 * precision - 1;
  return precision;
}

static inline unsigned int numero_bloques(unsigned int paso, unsigned int pos,
                                          unsigned int tip_pos)
{
  if (paso)
    return ((unsigned int)(pos * 0.5));
  if (!tip_pos && (pos % 2))
    return ((unsigned int)(pos * 0.75) + 1);
  return ((unsigned int)(pos * 0.75));
}

/***********************************************************************/
// Definicion de los metodos de la clase derivada
/***********************************************************************/

double BatchesDerived::quality(double relative_precision)
{
  unsigned int n, m;
  unsigned int i, j;
  unsigned int cuenta = 1;
  unsigned int tipo_cuenta = 1;

  unsigned int cuenta_aux = 1;
  unsigned int tipo_cuenta_aux = 1;

  unsigned int sumo = 0;

  double valor = 0.0;
  double valor_2 = 0.0;
  double valor_1 = 0.0;
  double media = 0.0;

  double *x = y;

  if (correlated)
    return (0.0);

  if (cheking)
    cheking = 0;

  m = numero_bloques(step, position, type) / GROUP_BATCHES;
  n = m * GROUP_BATCHES;

  for (j = 0; j < n - m + 1; j++)
  {

    for (i = 0; i < m; i++)
    {
      if (step)
      {
        if (!i)
          sumo += 2;
        valor += *x++;
        valor += *x++;
      }
      else
      {
        if (!i)
        {
          tipo_cuenta = tipo_cuenta_aux;
          cuenta = cuenta_aux;
          if (tipo_cuenta_aux)
          {
            sumo += 1;
            cuenta_aux++;
          }
          else
            sumo += 2;

          if ((tipo_cuenta_aux && (cuenta_aux == 2)) || (!tipo_cuenta_aux))
          {
            cuenta_aux = 0;
            tipo_cuenta_aux = !tipo_cuenta_aux;
          }
        }

        if (!tipo_cuenta)
        {
          valor += *x++;
          valor += *x++;
        }
        else
        {
          valor += *x++;
          cuenta++;
        }

        if ((tipo_cuenta && (cuenta == 2)) || (!tipo_cuenta))
        {
          cuenta = 0;
          tipo_cuenta = !tipo_cuenta;
        }
      }

      if ((!i) && (j != n - m))
        media += valor;
    }

    if (j == n - m)
      media += valor;

    x = y + sumo;

    valor_2 += valor * valor;
    valor_1 += valor;
    valor = 0.0;
  }

  media /= n;

  double varianza = 0.0;

  varianza += valor_2 / (m * m) - (2 * media * valor_1 / m) + (media * media * (n - m + 1));
  varianza /= ((double)n / m - 1) * (n - m + 1);

  if (varianza <= 0.0)
    return (t_quality(1E30 / 2));

  return (t_quality((relative_precision * fabs(media)) / sqrt(varianza)));
}

double BatchesDerived::confidence(double quality)
{
  double aux_quality = tval((1.0 + quality) * 0.5, 58);

  unsigned int n, m;
  unsigned int i, j;
  unsigned int cuenta = 1;
  unsigned int tipo_cuenta = 1;

  unsigned int tipo_cuenta_aux = 1;
  unsigned int cuenta_aux = 1;

  unsigned int sumo = 0;

  double valor = 0.0;
  double valor_2 = 0.0;
  double valor_1 = 0.0;
  double media = 0.0;

  double *x = y;

  if (correlated)
    return (MAXDOUBLE);

  if (cheking)
    cheking = 0;

  m = numero_bloques(step, position, type) / GROUP_BATCHES;
  n = m * GROUP_BATCHES;

  for (j = 0; j < n - m + 1; j++)
  {

    for (i = 0; i < m; i++)
    {
      if (step)
      {
        if (!i)
          sumo += 2;
        valor += *x++;
        valor += *x++;
      }
      else
      {
        if (!i)
        {
          tipo_cuenta = tipo_cuenta_aux;
          cuenta = cuenta_aux;
          if (tipo_cuenta_aux)
          {
            sumo += 1;
            cuenta_aux++;
          }
          else
            sumo += 2;

          if ((tipo_cuenta_aux && (cuenta_aux == 2)) || (!tipo_cuenta_aux))
          {
            cuenta_aux = 0;
            tipo_cuenta_aux = !tipo_cuenta_aux;
          }
        }

        if (!tipo_cuenta)
        {
          valor += *x++;
          valor += *x++;
        }
        else
        {
          valor += *x++;
          cuenta++;
        }

        if ((tipo_cuenta && (cuenta == 2)) || (!tipo_cuenta))
        {
          cuenta = 0;
          tipo_cuenta = !tipo_cuenta;
        }
      }

      if ((!i) && (j != n - m))
        media += valor;
    }

    if (j == n - m)
      media += valor;

    x = y + sumo;

    valor_2 += valor * valor;
    valor_1 += valor;
    valor = 0.0;
  }

  media /= n;

  double varianza = 0.0;

  varianza += valor_2 / (m * m) - (2 * media * valor_1 / m) + (media * media * (n - m + 1));
  varianza /= ((double)n / m - 1) * (n - m + 1);

  if (varianza <= 0.0)
    return (0.0);

  unsigned int l;
  l = step ? size * 3 : size * 2;

  return (aux_quality * sqrt(varianza) / l);
}
