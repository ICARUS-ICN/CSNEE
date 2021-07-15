/*******************************************************************/
//  Clase derivada de SampleStatistic que introduce un nuevo
// metodo de calculo de calidad alcanzada en la  estimacion
// de la media de las muestras
/******************************************************************/

#ifndef SMPLSTATDRVED_HH
#define SMPLSTATDRVED_HH

#include "SmplStat.h"

class SampleStatisticDerived : public SampleStatistic
{
public:
  double quality(double r_precision);
};

#endif
