#ifndef _Constant_h
#define _Constant_h 1

#include "Random.h"

//
//	The interval [lo..hi]
//

class Constant : public Random
{
  double pValue;

public:
  Constant(double value, RNG *gen);
  double value();
  double value(double x);

  virtual double operator()();
};

inline double Constant::operator()()
{
  return pValue;
}

#endif
