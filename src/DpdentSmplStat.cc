#define DPDENTSMPLSTAT_CC

#include "DpdentSmplStat.hh"
#include <iostream>
#include <stdlib.h>
using namespace std;

// Error handling

void default_DependentSampleStatistic_error_handler(const char *msg)
{
  cerr << "Fatal DependentSampleStatistic error. " << msg << "\n";
  exit(1);
}

//  one_arg_error_handler_t DependentSampleStatistic_error_handler
//  = default_DependentSampleStatistic_error_handler;

//  one_arg_error_handler_t set_DependentSampleStatistic_error_handler
//  (one_arg_error_handler_t f)
//  {
//    one_arg_error_handler_t old = DependentSampleStatistic_error_handler;
//    DependentSampleStatistic_error_handler = f;
//    return old;
//  }

/******************************************************************************/
//     IMPLEMENTACION DE LOS METODOS DE LA CLASE DEPENDENTSAMPLESTATISTIC
/*******************************************************************************/

unsigned short DependentSampleStatistic::num_averages = 0;

DependentSampleStatistic *DependentSampleStatistic::Pointer = NULL;

DependentSampleStatistic *DependentSampleStatistic::AggregatedMean = NULL;

#ifdef SEGUIMIENTO
unsigned short DependentSampleStatistic::real_averages = 0;
#endif

DependentSampleStatistic::DependentSampleStatistic(double coverage,
                                                   double relative_precision,
                                                   unsigned long min_samples)
{
  num_samples = 0;

  num_min = min_samples;

  if (coverage >= 1.0 || coverage < 0.0 || relative_precision <= 0.0)
    error("Wrong initialization\n");

  quality = coverage;
  current_quality = 0.0;
  precision = relative_precision;

  minValue = MAXDOUBLE;
  maxValue = 0.0;

  samples_cheking = quality > 0.0 ? 0 : ULONG_MAX;

  end = 0;

  DependentSampleStatistic *aux = Pointer;

  if (coverage > 0.0)
  {
    num_averages++;
    while ((aux != NULL) && (aux->next_object() != NULL))
      aux = aux->next_object();
    if (aux == NULL)
      Pointer = this;
    else
      aux->link_object(this);
  }

  next_pointer = NULL;

#ifdef SEGUIMIENTO
  order_averages = real_averages;
  real_averages++;
#endif
}

void DependentSampleStatistic::Check(void)
{
  current_quality = batch.quality(precision);

#ifdef SEGUIMIENTO
  cerr << "Order: " << order_averages << " ";
  cerr << "Samples: " << num_samples << " ";
  cerr << "Quality: " << current_quality << " ";
  //  cerr << "Mean: " << batch.mean();
#endif

  if (current_quality >= quality || num_samples == ULONG_MAX)
  {
    end = 1;
    samples_cheking = ULONG_MAX;
  }
  else
  {
    double t = batch.confidence(quality) / fabs(batch.mean());
    t /= precision;
    t *= t;
    unsigned long next_cheking = (unsigned long)((t + 1) * 0.5 * num_samples + 1);
    samples_cheking =
        (next_cheking > 2 * num_samples || next_cheking < num_samples) ? 2 * num_samples : next_cheking;
#ifdef SEGUIMIENTO
    cerr << " Estimation of needed samples : " << ceil(t * num_samples);
#endif
  }
#ifdef SEGUIMIENTO
  cerr << endl;
#endif
}

void DependentSampleStatistic::CheckAll(void)
{
  num_averages--;
  if (!num_averages)
  {
    DependentSampleStatistic *aux = Pointer;
    while (aux != NULL)
    {
      if (aux->coverage() < aux->ask_coverage() && aux->num_samples != ULONG_MAX)
        aux->restart();
      aux = aux->next_object();
    }
  }
  end = 0;
}

unsigned long int DependentSampleStatistic::samples()
{
  return (num_samples);
}

double DependentSampleStatistic::mean()
{
  return (batch.mean());
}

double DependentSampleStatistic::coverage()
{
  return (batch.quality(precision));
}

double DependentSampleStatistic::coverage(double relative_precision)
{
  return (batch.quality(relative_precision));
}

double DependentSampleStatistic::confidence()
{
  return (batch.confidence(quality));
}

double DependentSampleStatistic::confidence(int p_percentage)
{
  return (batch.confidence((double)p_percentage / 100));
}

double DependentSampleStatistic::confidence(double p_value)
{
  return (batch.confidence(p_value));
}

#ifdef COMPUTE_VARIANCE
double DependentSampleStatistic::stdDev()
{
  double variance = batch.var();

  if (variance <= 0.0)
    return (0.0);
  return (sqrt(variance));
}

double DependentSampleStatistic::var()
{
  double variance = batch.var();

  if (variance <= 0.0)
    return (0.0);
  return (variance);
}
#endif

double DependentSampleStatistic::min()
{
  return (minValue);
}

double DependentSampleStatistic::max()
{
  return (maxValue);
}

void DependentSampleStatistic::link_object(DependentSampleStatistic *ptr)
{
  next_pointer = ptr;
}

DependentSampleStatistic *DependentSampleStatistic::next_object()
{
  return (next_pointer);
}

double DependentSampleStatistic::ask_coverage()
{
  return (quality);
}

double DependentSampleStatistic::ask_relative_precision()
{
  return (precision);
}

void DependentSampleStatistic::start_aggregated_mean(double coverage,
                                                     double relative_precision,
                                                     unsigned long min_samples)
{
  if (AggregatedMean)
  {
    cerr << "Aggregated mean initialized already\n";
    return;
  }

  AggregatedMean =
      new DependentSampleStatistic(coverage, relative_precision, min_samples);

  if (!AggregatedMean)
  {
    cerr << "Memory allocation error\n";
    exit(1);
  }
}

void DependentSampleStatistic::show_aggregated_mean(ostream &stream)
{
  if (!AggregatedMean)
  {
    cerr << "Aggregated mean not initialized\n";
    return;
  }

  stream << (*AggregatedMean);
}

void DependentSampleStatistic::restart()
{
  samples_cheking = quality > 0.0 ? 0 : ULONG_MAX;
  end = 0;
  num_averages++;

  batch.restart();
}

void DependentSampleStatistic::error(const char *msg)
{
  cerr << msg << endl;
}

void DependentSampleStatistic::reset()
{

  num_samples = 0;

  current_quality = 0.0;

  if (end)
  {
    if (quality > 0.0)
      num_averages++;
    end = 0;
  }

  minValue = MAXDOUBLE;
  maxValue = 0.0;

  samples_cheking = 0;

  batch.reset();
}

DependentSampleStatistic::~DependentSampleStatistic()
{
}

ostream &operator<<(ostream &stream, DependentSampleStatistic average)
{
  double intervalo = average.confidence();
  double media = average.mean();
  double intervalo2 = average.ask_relative_precision();

  stream << media
         << "\n\tConfidence interval 1: (" << media - intervalo << ","
         << media + intervalo << ")"
         << " with coverage: " << average.ask_coverage()
         << "\n\tConfidence interval 2: (" << media - intervalo2 * fabs(media)
         << "," << media + intervalo2 * fabs(media) << ")"
         << " with coverage: " << average.coverage()
         << "\n\tNumber of samples: " << average.samples()
#ifdef COMPUTE_VARIANCE
         << "\n\tVariance: " << average.var()
         << "\n\tStandard deviation: " << average.stdDev()
#endif
         << "\n\tMaximum sample: " << average.max()
         << "\n\tMinimum sample: " << average.min() << "\n";

  return (stream);
}
