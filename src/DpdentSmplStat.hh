/********************************************************************************/
//     Clase que permite obtener los estadisticos de un conjunto de muestras
//  correlacionadas. Emplea la clase Batches, para eliminar la correlacion
//  entre las muestras y obtener una estimacion con una calidad
//  y tolerancia determinadas
/********************************************************************************/

#ifndef DPDENTSMPLSTAT_HH
#define DPDENTSMPLSTAT_HH

#include <limits.h>

//#define OVERLAPPING

//#define LRD

#ifdef LRD

#undef OVERLAPPING
#include "BatchesLRD.hh"

#else

#ifdef OVERLAPPING
#include "BatchesDrved.hh"
#else
#include "Batches.hh"
#endif

#endif

class DependentSampleStatistic
{
  static unsigned short num_averages;
  // Numero de promedios que no han alcanzado los requisitos.

  static DependentSampleStatistic *AggregatedMean;
  // Puntero a un objeto de la clase DependentSampleStatistic que lleva el
  // calculo del conjunto de muestras computadas por todos los promedios.
  // Se activa con la funcion start_aggregated_mean(...)

  unsigned long num_samples;
  // Numero de muestras.

  unsigned long num_min;
  // Numero minimo de muestras a computar.

  double minValue;
  // Minima muestra computada.
  double maxValue;
  // Maxima muestra computada.

  unsigned long samples_cheking;
  // Numero estimado de muestras para realizar una comprobacion
  // de la calidad.

  unsigned char end;
  // Toma el valor 1 mientras se han alcanzado los requisitos pero no
  // se computaron el minimo numero de muestras.

#ifdef OVERLAPPING
  BatchesDerived batch;
#else
  Batches batch;
#endif

  double quality;
  // Calidad pedida a la estimacion.
  double current_quality;
  // Calidad obtenida en el computo.
  double precision;
  // Tolerancia relativa a la media.

  DependentSampleStatistic *next_pointer;
  // Apunta al siguiente objeto de la clase, si existe, sino a NULL.
  // Se emplea para enlazar los objetos generados de la clase.

  static DependentSampleStatistic *Pointer;
  // Apunta al ultimo objeto de la clase que se ha generado.

  // Funciones que se emplean para recorrer los objetos enlazados y
  // para restablecer el calculo en el caso en el que no se cumplan los
  // requisitos.

  DependentSampleStatistic *next_object();
  // Devuelve el objeto que esta enlazado.

  void link_object(DependentSampleStatistic *ptr);
  // Enlaza el objeto que se pasa como parametro.

  void restart();
  // Reestablece una estimacion que no verifica los requisitos.

#ifdef SEGUIMIENTO
  static unsigned short real_averages;
  // Numero real de computos que se estan realizando.
  unsigned short order_averages;
  // Orden del promedio que visualiza un mensaje por pantalla.
#endif
  void Check(void);
  void CheckAll(void);

public:
  DependentSampleStatistic(double coverage = 0.0,
                           double relative_precision = 0.05,
                           unsigned long min_samples = 0);
  // Constructor de la clase DependentSampleStatistic.

  void operator+=(double sample);
  // Realiza el computo de una muestra real

  void yes(void);
  // En calculo de probabilidades computa un 1

  void no(void);
  // En calculo de probabilidades computa un 0

  static unsigned short number_averages()
  {
    return num_averages;
  }
  // Devuelve el numero de promedios por finalizar

  unsigned long samples();
  // Devuelve el numero de muestras.

  double mean();
  // Devuelve el valor de la media.

  double coverage();
  // Devuelve la calidad de la estimacion realizada para la tolerancia
  // pedida como requisito.
  double coverage(double relative_precision);
  // Devuelve la calidad de la estimacion realizada para la tolerancia
  // que se pasa como parametro.

  double confidence();
  // Devuelve el valor de la semilongitud del intervalo de confianza
  // con la calidad requerida.
  double confidence(int p_percentage);
  // Devuelve la semilongitud del intervalo de confianza de un p-porcentage.
  double confidence(double p_value);
  // Devuelve la semilongitud del intervalo de confianza de probabilidad p.

  double ask_coverage();
  // Devuelve la calidad requerida en la estimacion.
  double ask_relative_precision();
  // Devuelve la tolerancia relativa requerida.

#ifdef COMPUTE_VARIANCE
  double stdDev();
  // Devuelve la desviacion standard de las muestras.
  double var();
  // Devuelve el valor de la varianza.
#endif

  double min();
  // Devuelve la minima muestra computada.
  double max();
  // Devuelve la maxima muestra computada.

  void error(const char *msg);
  // Funcion de salida de los mensajes de error.
  void reset();
  // Reinicializa un objeto de la clase.

  static void start_aggregated_mean(double coverage = 0.0,
                                    double relative_precision = 0.05,
                                    unsigned long min_samples = 0);
  // Permite que se realice el calculo de la media agregada.

  static void show_aggregated_mean(ostream &stream);
  // Permite obtener los mensajes de salida de la media agregada.

  ~DependentSampleStatistic();
  // Destructor de la clase.

  friend ostream &operator<<(ostream &stream, DependentSampleStatistic average);
  // Sobrecarga del operador de insercion.
  // Vuelca el valor de la media, intervalo de confianza, calidad alcanzada,
  // numero de muestras computadas, varianza, desviacion standard, maxima
  // y minima muestra computadas.
};

// error handlers
//extern  void default_DependentSampleStatistic_error_handler(const char *);
//extern  one_arg_error_handler_t DependentSampleStatistic_error_handler;

//extern  one_arg_error_handler_t
//        set_DependentSampleStatistic_error_handler(one_arg_error_handler_t f);

/*************************************************************************/
//           Funcion inline de la clase DependentSampleStatistic
/*************************************************************************/

inline void DependentSampleStatistic::operator+=(double sample)
{
  num_samples++;

  if (!num_samples)
  {
    num_samples--;
    if (!end)
    {
      Check();
      CheckAll();
    }
    end = 1;
    return;
  }

  if (AggregatedMean && this != AggregatedMean)
    (*AggregatedMean) += sample;

  if (sample < minValue)
    minValue = sample;
  if (sample > maxValue)
    maxValue = sample;

  bool cheking = batch.compute(sample);
  // Indica cuando se pueden hacer estimaciones de calidad alcanzada.
  if (cheking && num_samples >= samples_cheking)
    Check();

  if (end && num_samples >= num_min)
    CheckAll();
}

inline void DependentSampleStatistic::yes(void)
{
  num_samples++;

  if (!num_samples)
  {
    num_samples--;
    if (!end)
    {
      Check();
      CheckAll();
    }
    end = 1;
    return;
  }

  if (AggregatedMean && this != AggregatedMean)
    (*AggregatedMean).yes();

  if (minValue > 1)
    minValue = 1;
  if (1 > maxValue)
    maxValue = 1;

  bool cheking = batch.yes();
  // Indica cuando se pueden hacer estimaciones de calidad alcanzada.
  if (cheking && num_samples >= samples_cheking)
    Check();

  if (end && num_samples >= num_min)
    CheckAll();
}

inline void DependentSampleStatistic::no(void)
{
  num_samples++;

  if (!num_samples)
  {
    num_samples--;
    if (!end)
    {
      Check();
      CheckAll();
    }
    end = 1;
    return;
  }

  if (AggregatedMean && this != AggregatedMean)
    (*AggregatedMean).no();

  if (0 < minValue)
    minValue = 0;
  if (0 > maxValue)
    maxValue = 0;

  bool cheking = batch.no();
  // Indica cuando se pueden hacer estimaciones de calidad alcanzada.
  if (cheking && num_samples >= samples_cheking)
    Check();

  if (end && num_samples >= num_min)
    CheckAll();
}

#endif
