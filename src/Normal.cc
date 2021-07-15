/* 
Copyright (C) 1988 Free Software Foundation
    written by Dirk Grunwald (grunwald@cs.uiuc.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include "Normal.h"
//
//	See Simulation, Modelling & Analysis by Law & Kelton, pp259
//
//	This is the ``polar'' method.
//

Normal::Normal(double xmean, double xvariance, RNG *gen)
    : Random(gen)
{
  pMean = xmean;
  pVariance = xvariance;
  pStdDev = sqrt(pVariance);
  haveCachedNormal = 0;
  if (pMean == 0 && pVariance == 1)
    standard = 1;
  else
    standard = 0;
}

double Normal::mean() { return pMean; };
double Normal::mean(double x)
{
  double t = pMean;
  pMean = x;
  if (pMean == 0 && pVariance == 1)
    standard = 1;
  else
    standard = 0;
  return t;
}

double Normal::variance() { return pVariance; }
double Normal::variance(double x)
{
  double t = pVariance;
  pVariance = x;
  pStdDev = sqrt(pVariance);
  if (pMean == 0 && pVariance == 1)
    standard = 1;
  else
    standard = 0;
  return t;
};
