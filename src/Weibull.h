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
#ifndef _Weibull_h
#define _Weibull_h

#include "Random.h"

class Weibull : public Random
{
protected:
    double pAlpha;
    double pInvAlpha;
    double pBeta;

    void setState();

public:
    Weibull(double alpha, double beta, RNG *gen);

    double alpha();
    double alpha(double x);

    double beta();
    double beta(double x);

    virtual double operator()();
};

//
//	See Simulation, Modelling & Analysis by Law & Kelton, pp259
//
//	This is the ``polar'' method.
//

inline double Weibull::operator()()
{
    return (pow(pBeta * (-log(1 - pGenerator->asDouble())), pInvAlpha));
}

#endif
