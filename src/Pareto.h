#ifndef _Pareto_h
#define _Pareto_h

#include "Random.h"

class Pareto : public Random
{
protected:
  double pMin;
  double pInvAlfa;

public:
  Pareto(double xmin, double xalfa, RNG *gen, unsigned value = 0);
  virtual double operator()();
};

inline Pareto::Pareto(double xmin, double xalfa, RNG *gen, unsigned value)
    : Random(gen)
{
  pMin = xmin;
  pInvAlfa = -1 / xalfa;
}

inline double Pareto::operator()()
{
#ifdef MPINF_FLOAT
  return pMin * pow(pGenerator->asFloat(), pInvAlfa);
#else
  return pMin * pow(pGenerator->asDouble(), pInvAlfa);
#endif
}
#endif
