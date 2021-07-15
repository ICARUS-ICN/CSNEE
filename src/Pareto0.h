#ifndef _Pareto0_h
#define _Pareto0_h

#include "Random.h"

class Pareto0 : public Random
{
protected:
  double pMin;
  double pInvAlfa;

public:
  Pareto0(double xmin, double xalfa, RNG *gen, unsigned value = 0);
  virtual double operator()();
};

inline Pareto0::Pareto0(double xmin, double xalfa, RNG *gen, unsigned value)
    : Random(gen)
{
  pMin = xmin;
  pInvAlfa = -1 / xalfa;
}

inline double Pareto0::operator()()
{
#ifdef MPINF_FLOAT
  return pMin * pow(pGenerator->asFloat(), pInvAlfa) - pMin;
#else
  return pMin * pow(pGenerator->asDouble(), pInvAlfa) - pMin;
#endif
}
#endif
