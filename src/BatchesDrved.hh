#ifndef BATCHESDRVED_HH
#define BATCHESDRVED_HH

#include "Batches.hh"

class BatchesDerived : public Batches
{
public:
  double quality(double relative_precision);
  // Calculo de la calidad por el metodo de overlapping.

  double confidence(double quality);
  // Calculo del semiintervalo de tolerancia por el metodo overlapping.
};

#endif
