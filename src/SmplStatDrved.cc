#define SMPLSTATDRVED_CC

#include "SmplStatDrved.hh"
#include <math.h>

static double fiz[] = {
    0.5, 0.0,
    0.6, 0.254657,
    0.7, 0.528295,
    0.8, 0.850746,
    0.9, 1.30381,
    0.9333, 1.5331,
    0.95, 1.68526,
    0.96, 1.79795,
    0.9667, 1.88761,
    0.9750, 2.0232,
    0.98, 2.12528,
    0.9833, 2.2058,
    0.9875, 2.33185,
    0.99, 2.42644,
    0.9917, 2.50392,
    0.9938, 2.62272,
    0.9950, 2.70855};

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

  if (precision > 2.70855 && precision < 1E30 / 3.0)
  {

    while (tval(p, 39) < precision)
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
          (delta_precision / (tval(p, 39) - tval(p - delta_precision, 39))) *
              (precision - tval(p - delta_precision, 39)) +
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

/*********************************************************************************/
//             METODO DE LA CLASE DERIVADA DE SAMPLESTATISTIC
/*********************************************************************************/

double SampleStatisticDerived::quality(double r_precision)
{
  double varianza = var();

  if (varianza <= 0.0)
    return (t_quality(1E30 / 2.0));

  return (t_quality(r_precision * fabs(mean()) / sqrt(varianza / samples())));
}
