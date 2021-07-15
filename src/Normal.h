// This may look like C code, but it is really -*- C++ -*-
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
#ifndef _Normal_h
#define _Normal_h

#include "Random.h"

class Normal : public Random
{
	char haveCachedNormal;
	double cachedNormal;

protected:
	double pMean;
	double pVariance;
	double pStdDev;
	int standard;

public:
	Normal(double xmean, double xvariance, RNG *gen);
	double mean();
	double mean(double x);
	double variance();
	double variance(double x);
	virtual double operator()();
};

inline double Normal::operator()()
{

	if (haveCachedNormal == 1)
	{
		haveCachedNormal = 0;
		return (standard ? cachedNormal : cachedNormal * pStdDev + pMean);
	}
	else
	{

		for (;;)
		{
			double u1 = pGenerator->asDouble();
			double u2 = pGenerator->asDouble();
			double v1 = 2 * u1 - 1;
			double v2 = 2 * u2 - 1;
			double w = (v1 * v1) + (v2 * v2);

			//
			//	We actually generate two IID normal distribution variables.
			//	We cache the one & return the other.
			//
			if (w <= 1)
			{
				double y = sqrt((-2 * log(w)) / w);
				double x1 = v1 * y;
				double x2 = v2 * y;

				haveCachedNormal = 1;
				cachedNormal = x2;
				return (standard ? x1 : x1 * pStdDev + pMean);
			}
		}
	}
}

#endif
