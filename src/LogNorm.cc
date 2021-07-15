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

#include "LogNorm.h"

//
//	See Simulation, Modelling & Analysis by Law & Kelton, pp260
//
//

void LogNormal::setState()
{
    double m2 = logMean * logMean;
    pMean = log(m2 / sqrt(logVariance + m2));
    // from ch@heike.informatik.uni-dortmund.de:
    // (was   pVariance = log((sqrt(logVariance + m2)/m2 )); )
    pStdDev = sqrt(log((logVariance + m2) / m2));
}

LogNormal::LogNormal(double mean, double variance, RNG *gen)
    : Normal(mean, variance, gen)
{
    logMean = mean;
    logVariance = variance;
    setState();
}

double LogNormal::mean()
{
    return logMean;
}

double LogNormal::mean(double x)
{
    double t = logMean;
    logMean = x;
    setState();
    return t;
}

double LogNormal::variance()
{
    return logVariance;
}

double LogNormal::variance(double x)
{
    double t = logVariance;
    logVariance = x;
    setState();
    return t;
}