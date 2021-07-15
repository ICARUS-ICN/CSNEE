// This may look like C code, but it is really -*- C++ -*-
/* 
Copyright (C) 1989 Free Software Foundation

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

#include <assert.h>
//#include <builtin.h>
#include "MRG.h"
#include <sys/time.h>

#ifdef DEBUG
#include <iostream>
using namespace std;

//#include <stream.h>
#endif

// These two static fields get initialized by MRG::MRG().
PrivateSingleType MRG::singleMantissa;
PrivateDoubleType MRG::doubleMantissa;

//
//	The scale constant is 2^-31. It is used to scale a 31 bit
//	long to a double.
//

//static const double randomDoubleScaleConstant = 4.656612873077392578125e-10;
//static const float  randomFloatScaleConstant = 4.656612873077392578125e-10;

static char initialized = 0;

MRG::MRG(int32_t seed)
{
	if (!initialized)
	{

		assert(sizeof(double) == 2 * sizeof(uint32_t));

		//
		//	The following is a hack that I attribute to
		//	Andres Nowatzyk at CMU. The intent of the loop
		//	is to form the smallest number 0 <= x < 1.0,
		//	which is then used as a mask for two longwords.
		//	this gives us a fast way way to produce double
		//	precision numbers from longwords.
		//
		//	I know that this works for IEEE and VAX floating
		//	point representations.
		//
		//	A further complication is that gnu C will blow
		//	the following loop, unless compiled with -ffloat-store,
		//	because it uses extended representations for some of
		//	of the comparisons. Thus, we have the following hack.
		//	If we could specify #pragma optimize, we wouldn't need this.
		//

		PrivateDoubleType t;
		PrivateSingleType s;

#if _IEEE == 1

		t.d = 1.5;
		if (t.u[1] == 0)
		{ // sun word order?
			t.u[0] = 0x3fffffff;
			t.u[1] = 0xffffffff;
		}
		else
		{
			t.u[0] = 0xffffffff; // encore word order?
			t.u[1] = 0x3fffffff;
		}

		s.u = 0x3fffffff;
#else
		volatile double x = 1.0; // volatile needed when fp hardware used,
								 // and has greater precision than memory doubles
		double y = 0.5;
		do
		{ // find largest fp-number < 2.0
			t.d = x;
			x += y;
			y *= 0.5;
		} while (x != t.d && x < 2.0);

		volatile float xx = 1.0; // volatile needed when fp hardware used,
								 // and has greater precision than memory floats
		float yy = 0.5;
		do
		{ // find largest fp-number < 2.0
			s.s = xx;
			xx += yy;
			yy *= 0.5;
		} while (xx != s.s && xx < 2.0);
#endif
		// set doubleMantissa to 1 for each doubleMantissa bit
		doubleMantissa.d = 1.0;
		doubleMantissa.u[0] ^= t.u[0];
		doubleMantissa.u[1] ^= t.u[1];
#if 1 //#cpu(i386) || #cpu(x86_64) FIXME
		assert(doubleMantissa.u[0] == 0xFFFFFFFF);
#else
		assert(doubleMantissa.u[1] == 0xFFFFFFFF);
#endif
#ifdef DEBUG
		cout << "doubleMantissa.u[0 y 1]: " << form("%x ", doubleMantissa.u[0]) << form("%x", doubleMantissa.u[1]) << endl;
#endif

		// set singleMantissa to 1 for each singleMantissa bit
		singleMantissa.s = 1.0;
		singleMantissa.u ^= s.u;

#ifdef DEBUG
		cout << "singleMantissa.u: " << form("%x ", singleMantissa.u) << endl;
#endif

		initialized = 1;
	}

	struct timeval tv;
	if (!seed)
	{
		gettimeofday(&tv, NULL);
		seed = tv.tv_sec;
	}

	if (seed < 0)
		seed &= 0x7FFFFFFF;
	if ((unsigned long)seed >= M2)
		seed &= 0x3FFFFFFF;
	if (!seed)
		seed = 0x55555555;

	initialSeed = seed;
	reset();
}

void MRG::reset()
{
	x10 = initialSeed;
	x11 = initialSeed ^ 0x15555555;
	if (x11 >= M2)
		x11 &= 0x3FFFFFFF;
	x12 = initialSeed ^ 0x2AAAAAAA;
	if (x12 >= M2)
		x12 &= 0x3FFFFFFF;
	x20 = ((initialSeed << 15) + (initialSeed >> 15));
	if (x20 >= M2)
		x20 &= 0x3FFFFFFF;
	x21 = x20 ^ 0x2AAAAAAA;
	if (x21 >= M2)
		x21 &= 0x3FFFFFFF;
	x22 = x20 ^ 0x15555555;
	if (x22 >= M2)
		x22 &= 0x3FFFFFFF;
}

inline void
MRG::seed(int32_t s)
{
	initialSeed = s;
	reset();
}

inline void
MRG::reseed(int32_t s)
{
	initialSeed = s;
	reset();
}
