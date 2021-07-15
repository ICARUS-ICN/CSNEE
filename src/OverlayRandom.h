#ifndef _OverlayRandom_h
#define _OverlayRandom_h 1

#include "Random.h"

#include "Uniform.h"

#include <queue>
using namespace std;

//
//	The interval [lo..hi]
//

class OverlayRandom : public Random
{
  priority_queue<double> p_queue;
  Random *source;
  unsigned pNumber;
  double clock;

public:
  OverlayRandom(unsigned number, Random *Source, RNG *gen,
                Random *residual = NULL);

  unsigned number();

  virtual double operator()();
};

inline double OverlayRandom::operator()()
{
  double x = -p_queue.top();
  p_queue.pop();
  p_queue.push(-(x + (*source)()));

  double temp = x - clock;
  clock = x;
  return temp;
}
#endif
