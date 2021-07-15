/**********************************************************************************/
//  Clase que implementa el metodo de bloques de Law & Carson para realizar una
// estimacion de la media de un conjunto de muestras en las que existe correlacion
//  Se emplea la clase SampleStatistic para calcular los estadisticos
// de la agrupacion de bloques que se consideran independientes.
/**********************************************************************************/
#ifndef BATCHES_HH
#define BATCHES_HH

#include <fstream>
#include <math.h>
#include <values.h>
using namespace std;

#include "SmplStatDrved.hh"

// Version: @(#)Batches.hh	1.1 01/14/11

#define SEGUIMIENTO
// Activa la salida de mensajes por la salida de error.
#define COMPUTE_VARIANCE
// Activa computo de varianza y desviacion tipica

// Parametros del algoritmo que se pueden modificar.
#define NUMBER_BATCHES 400
#define LENGTH_GROUP 10

// Parametros de la implementacion que se obtienen a partir de los anteriores.
#define LEVEL (-0.388888 * (1.0 / LENGTH_GROUP) + 0.438888)
#define LENGTH_ARRAY (2 * NUMBER_BATCHES)
#define GROUP_BATCHES ((unsigned int)(NUMBER_BATCHES / LENGTH_GROUP))
#if ((NUMBER_BATCHES % 3) == 2)
#define LENGTH_EVEN ((unsigned int)(NUMBER_BATCHES * 4 / 3) + 1)
#else
#define LENGTH_EVEN ((unsigned int)(NUMBER_BATCHES * 4 / 3))
#endif

class Batches
{
protected:
  // Variables necesarias para el almacenamiento en la estructura.
  double sum_samples;
  // Acumulador de la suma de las muestras, mientras no se almacenan.
  unsigned int sum_samples_bool;

  unsigned long cont_samples;
  // Numero de muestras almacenadas en el acumulador.
  unsigned char type;
  // Tipo de posicion en la que se esta almacenando.
  unsigned char cont_type;
  // Numero de posiciones del mismo tipo consecutivas.

  unsigned long num_samples;
  // Numero de muestras computadas.

  unsigned char join;
  // Indica comprimir el vector de muestras
  unsigned char correlated;
  // Indica si los bloques estan correlacionados o no.

#ifdef COMPUTE_VARIANCE
  double sum_q;
  unsigned int sum_q_bool;
  // Se emplea para calcular el valor de la varianza de las muestras.
#endif

  unsigned short position;
  // Posicion de almacenamiento en el vector y.
  unsigned char step;
  // Indica si se esta trabajando con bloques de tamanho
  // multiplo de 2 (0) o de 3 (1).
  unsigned long size, size_check;
  // Indica el numero de muestras almacenadas en una posicion simple.

  unsigned long samples_cheking;
  // Indica cuando tenemos una nueva agrupacion de 40 bloques para
  // realizar una comprobacion de calidad o de tolerancia.
  bool cheking;
  // Su valor se mantiene a 1 mientras se pueda realizar una comprobacion y
  // no se realice. La comprobacion se puede realizar estimando la calidad
  // o el semiintervalo de tolerancia.

  double y[LENGTH_ARRAY];

  SampleStatisticDerived average;
  // Objeto que permite el calculo de los estadisticos de los bloques.
  void AverageSamples(void);
  // Pass current batch samples to object average.

  double r1;
  // Valor del primer coeficiente de autocorrelacion.
  double r1_aux;
  // Lo necesito para comprobar si es decreciente.

  unsigned long longitud;
  // Empleado para el calculo del semiintervalo de tolerancia.

#ifdef SEGUIMIENTO
  static unsigned short real_promedios;
  // Numero real de computos que se estan realizando.
  unsigned short order_averages;
  // Orden del promedio que visualiza un mensaje por pantalla.
#endif
  void Join(void);
  void CheckR1(void);
  void CheckPoint(void);

  bool compute_common(void);

public:
  Batches();
  // Constructor de la clase Batches.

  bool compute(double sample);
  // Almacena la muestra, calcula el factor de autocorrelacion
  // y comprueba si los bloques estan correlacionados.
  // Devuelve un 1 si podemos hacer una agrupacion de las muestras en 40
  // bloques en los que la correlacion es despreciable, y sigue devolviendo
  // este valor mientras no se realice una comprobacion de calidad o de tolerancia
  // sobre ese conjunto de bloques.
  bool yes(void);
  // idem. pero añade un 1
  bool no(void);
  // idem. pero añade un 0

  virtual double quality(double relative_precision);
  // Calcula la calidad obtenida en el computo respecto
  // a una tolerancia relativa dada
  // Si los bloques estan correlacionados se devuelve 0.0.

  virtual double confidence(double quality);
  // Devuelve el valor del semiintervalo de confianza para un valor
  // de calidad dado
  // Si los bloques estan correlacionados devuelve MAXDOUBLE.

  double mean();
  // Devuelve la media de muestras almacenadas.

#ifdef COMPUTE_VARIANCE
  double var();
  // Devuelve la varianza de las muestras almacenadas.
#endif

  void restart();
  // Permite que se vuelvan a realizar calculos de la correlacion entre
  // los bloques.

  void reset();
  // Reinicializa un objeto de la clase Batches.

  virtual ~Batches();
  // Destructor de la clase Batches.
};

inline bool Batches::compute_common(void)
{
  cont_samples++;

  // Se agrupan las muestras
  if (join)
    Join();

  // Almacena la muestra
  if (cont_samples == size_check)
  {
    y[position++] = sum_samples + sum_samples_bool;
    if (position == LENGTH_ARRAY)
      join = 1;
    sum_samples = 0.0;
    sum_samples_bool = 0;
    num_samples += cont_samples;
    cont_samples = 0;
    cont_type++;
    if (cont_type == 2)
    {
      cont_type = 0;
      type = !type;
      size_check = type ? size * 2 : size;
    }
    if (correlated)
    {
      if (position == LENGTH_EVEN || position == LENGTH_ARRAY)
        CheckR1();
    }
    else
        // Analiza cuando se pueden hacer nuevas comprobaciones
        if (samples_cheking == num_samples)
      CheckPoint();
  }
  return (cheking);
}

inline bool Batches::compute(double sample)
{
  sum_samples += sample;

#ifdef COMPUTE_VARIANCE
  sum_q += sample * sample;
#endif

  return compute_common();
}

inline bool Batches::yes(void)
{
  sum_samples_bool++;

#ifdef COMPUTE_VARIANCE
  sum_q_bool++;
#endif

  return compute_common();
}

inline bool Batches::no(void)
{
  return compute_common();
}
#endif
