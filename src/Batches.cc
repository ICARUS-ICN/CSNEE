#define BATCHES_CC

#include "Batches.hh"
#include <iostream>
using namespace std;

// Reduce a la mitad el vector origen en destino, siguiendo la misma estructura

static void agrupa(double *origen, double *destino, unsigned int pos)
{
  unsigned int tipo_pos = 1;
  unsigned int cuenta = 1;
  unsigned int num_suma;
  double suma = 0.0;
  unsigned int j, contador = 0, indice = 0;

  while (contador < pos)
  {
    num_suma = tipo_pos ? 3 : 1;
    for (j = 0; j < num_suma; j++)
      suma += origen[contador++];
    destino[indice++] = suma;
    suma = 0.0;
    cuenta++;
    if (cuenta == 2)
    {
      cuenta = 0;
      tipo_pos = !tipo_pos;
    }
  }
}

// Calcula el primer coeficiente de autocorrelacion de n bloques almacenados
// en el vector p, el tamanho de los bloques es multiplo de 2 o de 3
// segun el valor de paso

static double calcula_r1(double *p, unsigned int n, unsigned int paso)
{
  double actual = 0.0;
  double anterior = 0.0;
  double media = 0.0;
  double sum1 = 0.0, sum1_q = 0.0, sum1_dq = 0.0, primero1 = 0.0, ultimo1 = 0.0;
  double sum2 = 0.0, sum2_q = 0.0, sum2_dq = 0.0, primero2 = 0.0, ultimo2 = 0.0;
  double valor1_r1, valor2_r1, r1;
  unsigned int bloques, j;
  unsigned int cuenta = 1;
  unsigned int tipo = 1;

  double *x = p;
  int iguales = 1;

  for (bloques = 0; bloques < n / 2; bloques++)
  {
    actual = 0.0;
    if (paso)
      for (j = 0; j < 2; j++)
        actual += *x++;
    else
    {
      if (!tipo)
      {
        actual += *x++;
        actual += *x++;
      }
      else
      {
        actual += *x++;
        cuenta++;
      }
      if ((tipo && (cuenta == 2)) || (!tipo))
      {
        cuenta = 0;
        tipo = !tipo;
      }
    }

    if (!bloques)
      primero1 = actual;

    sum1 += actual;
    sum1_q += actual * actual;
    sum1_dq += actual * anterior;
    if (iguales && bloques && anterior != actual)
      iguales = 0;
    anterior = actual;
  }

  if (iguales)
    valor1_r1 = 0.0; // Todas las muestras iguales
  else
  {
    ultimo1 = actual;
    media = sum1 / bloques;
    valor1_r1 = sum1_dq + media * (primero1 + ultimo1 - (bloques + 1) * media);
    valor1_r1 /= sum1_q - bloques * media * media;
  }

  iguales = 1;
  anterior = 0.0;
  for (bloques = 0; bloques < n / 2; bloques++)
  {
    actual = 0.0;
    if (paso)
      for (j = 0; j < 2; j++)
        actual += *x++;
    else
    {
      if (!tipo)
      {
        actual += *x++;
        actual += *x++;
      }
      else
      {
        actual += *x++;
        cuenta++;
      }
      if ((tipo && (cuenta == 2)) || (!tipo))
      {
        cuenta = 0;
        tipo = !tipo;
      }
    }

    if (!bloques)
      primero2 = actual;

    sum2 += actual;
    sum2_q += actual * actual;
    sum2_dq += actual * anterior;
    if (iguales && bloques && anterior != actual)
      iguales = 0;
    anterior = actual;
  }

  if (iguales)
    valor2_r1 = 0.0; // Todas las muestras iguales
  else
  {
    ultimo2 = actual;
    media = sum2 / bloques;
    valor2_r1 = sum2_dq + media * (primero2 + ultimo2 - (bloques + 1) * media);
    valor2_r1 /= sum2_q - bloques * media * media;
  }

  if ((valor1_r1 == 0.0) && (valor2_r1 == 0.0))
    return (0.0);

  media = (sum1 + sum2) / n;
  r1 = sum1_dq + sum2_dq + (ultimo1 * primero2) +
       media * (primero1 + ultimo2 - (n + 1) * media);
  r1 /= sum1_q + sum2_q - n * media * media;

  return ((2 * r1 - (valor1_r1 + valor2_r1) * 0.5));
}

// Sirve para calcular el numero de bloques de tamanho multiplo
// de 2 o de 3 que tenemos almacenados en la estructura

static inline unsigned int numero_bloques(unsigned int paso, unsigned int pos,
                                          unsigned int tip_pos)
{
  if (paso)
    return ((unsigned int)(pos * 0.5));
  if (!tip_pos && (pos % 2))
    return ((unsigned int)(pos * 0.75) + 1);
  return ((unsigned int)(pos * 0.75));
}

/****************************************************************************/
//                     METODOS DE LA CLASE BATCHES
/****************************************************************************/

#ifdef SEGUIMIENTO
unsigned short Batches::real_promedios = 0;
#endif

Batches::Batches()
{
  r1 = 1.0;
  r1_aux = 0.0;

  correlated = 1;

  num_samples = 0;

#ifdef COMPUTE_VARIANCE
  sum_q = 0.0;
  sum_q_bool = 0;
#endif

  position = 0;
  size = 1;
  step = 0;
  join = 0;

  type = 1;
  size_check = type ? size * 2 : size;
  cont_type = 1;
  sum_samples = 0.0;
  sum_samples_bool = 0;
  cont_samples = 0;

  samples_cheking = 0;
  cheking = 0;

#ifdef SEGUIMIENTO
  order_averages = real_promedios;
  real_promedios++;
#endif
}

void Batches::Join(void)
{
  join = 0;
  agrupa(y, y, position);
  position = NUMBER_BATCHES;
  type = 1;
  size *= 2;
  size_check = type ? size * 2 : size;
}

void Batches::CheckR1(void)
{
  // Analisis de la correlacion de los bloques
  double x[NUMBER_BATCHES];
  double r1_actual;

  step = (position == LENGTH_EVEN) ? 0 : 1;
  r1_actual = calcula_r1(y, NUMBER_BATCHES, step);

#ifdef SEGUIMIENTO
  cerr << "Order: " << order_averages << " ";
  cerr << "Samples: " << num_samples;
  cerr << "  r1(" << NUMBER_BATCHES << ",";
  if (step)
    cerr << size * 3;
  else
    cerr << size * 2;
  cerr << ") = " << r1_actual << "\n";
#endif
  agrupa(y, x, position);
  if (r1_actual <= 0.0 ||
      (r1_actual < LEVEL &&
       (r1_aux > r1_actual || r1_actual > calcula_r1(x, (NUMBER_BATCHES / 2), step))))
    correlated = 0;

  r1_aux = r1;
  r1 = r1_actual;

  if (correlated)
    return;
  samples_cheking = step ? num_samples + size * 6 * GROUP_BATCHES : num_samples + size * 2 * GROUP_BATCHES;
  cheking = 1;
}

void Batches::CheckPoint(void)
{
  // This is a check point
  cheking = 1;

  // Computes next check point size
  // only multiples of 2 or 3 -> this could be improved checking both
  if (step)
  {
    samples_cheking = num_samples + size * 3 * GROUP_BATCHES;
    if (numero_bloques(step, position, type) == NUMBER_BATCHES)
      samples_cheking = num_samples + size * 6 * GROUP_BATCHES;
  }
  else
    samples_cheking = num_samples + size * 2 * GROUP_BATCHES;
}

void Batches::AverageSamples(void)
{
  unsigned int grupo;
  unsigned int i, j;
  double valor = 0.0;
  unsigned int cuenta = 1;
  unsigned int tipo_cuenta = 1;

  double *x = y;
  grupo = numero_bloques(step, position, type) / GROUP_BATCHES;

  longitud = step ? grupo * size * 3 : grupo * size * 2;

  for (i = 0; i < GROUP_BATCHES; i++)
  {
    if (step)
      for (j = 0; j < (grupo * 2); j++)
        valor += *x++;
    else
    {
      for (j = 0; j < grupo; j++)
      {
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
    }

    average += valor;
    valor = 0.0;
  }
}

double Batches::quality(double relative_precision)
{
  if (correlated)
    return (0.0);

  if (cheking)
  {
    cheking = 0;
    average.reset();
  }
  else
    return (average.quality(relative_precision));

  AverageSamples();
  return (average.quality(relative_precision));
}

double Batches::confidence(double quality)
{
  if (correlated)
    return (MAXDOUBLE);

  if (cheking)
  {
    cheking = 0;
    average.reset();
  }
  else
    return ((double)(average.confidence(quality) / longitud));

  AverageSamples();
  return ((double)(average.confidence(quality) / longitud));
}

double Batches::mean()
{

  unsigned int i;
  double aux_mean = 0.0;
  double *x;

  if (!(num_samples + cont_samples))
    return (0.0);

  for (i = 0, x = y; i < position; i++)
    aux_mean += *x++;
  aux_mean += sum_samples + sum_samples_bool;
  aux_mean /= num_samples + cont_samples;

  return (aux_mean);
}

#ifdef COMPUTE_VARIANCE
double Batches::var()
{
  double aux_mean = mean();

  if (num_samples + cont_samples <= 1)
    return (0.0);
  return (sum_q + sum_q_bool - aux_mean * aux_mean * (num_samples + cont_samples)) / (num_samples + cont_samples - 1);
}
#endif

void Batches::reset()
{
  r1 = 1.0;
  r1_aux = 0.0;

  correlated = 1;

  num_samples = 0;

#ifdef COMPUTE_VARIANCE
  sum_q = 0.0;
  sum_q_bool = 0;
#endif

  position = 0;
  step = 0;
  size = 1;
  join = 0;

  samples_cheking = 0;
  cheking = 0;

  type = 1;
  size_check = type ? size * 2 : size;
  cont_type = 1;
  sum_samples = 0.0;
  sum_samples_bool = 0;
  cont_samples = 0;
}

void Batches::restart()
{
  correlated = 1;
}

Batches::~Batches()
{
}
