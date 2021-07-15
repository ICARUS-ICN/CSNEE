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
#ifndef _MRG_h
#define _MRG_h 1

#include <assert.h>
#include <math.h>

#include <stdint.h>

union PrivateSingleType
{ // used to access floats as unsigneds
  float s;
  uint32_t u;
};

union PrivateDoubleType
{ // used to access doubles as unsigneds
  double d;
  uint32_t u[2];
};

//
// Base class for Random Number Generators. See ACG and MRG for instances.
//
class MRG
{
  static PrivateSingleType singleMantissa; // mantissa bit vector
  static PrivateDoubleType doubleMantissa; // mantissa bit vector

  int32_t initialSeed;
  int32_t x10, x11, x12, x20, x21, x22;

public:
  // if seed = 0 then random seed
  MRG(int32_t seed = 0);
  void seed(int32_t);
  void reseed(int32_t);
  //
  // Return a long-words word of random bits
  //
  uint32_t asLong();
  void reset();
  unsigned long sup();
  //
  // Return random bits converted to either a float or a double
  //
  float asFloat();
  double asDouble();
};

#define M1 2147483647U
#define M2 2147462579U
#define MASK12 511U
#define MASK13 16777215U
#define MASK21 65535U
inline uint32_t MRG::asLong()
{
  unsigned long y1 = (((x11 & MASK12) << 22) + (x11 >> 9)) + (((x12 & MASK13) << 7) + (x12 >> 24));
  if (y1 > M1)
    y1 -= M1;
  y1 += x12;
  if (y1 > M1)
    y1 -= M1;
  x12 = x11;
  x11 = x10;
  x10 = y1;

  unsigned long ya = 21069 * ((x20 >> 16) + (x22 >> 16));
  unsigned long y2 = (((x22 & MASK21) + (x20 & MASK21)) << 15);
  if (y2 > M2)
    y2 -= M2;
  // Original code:
  //    unsigned long ya = ((x20 & MASK21)<<15) + 21069 * (x20>>16);
  //    if (ya>M2) ya -= M2;
  //    unsigned long y2 = ((x22 & MASK21)<<15) + 21069 * (x22>>16);
  //    if (y2>M2) y2 -= M2;

  y2 += x22;
  if (y2 > M2)
    y2 -= M2;
  y2 += ya;
  if (y2 > M2)
    y2 -= M2;
  x22 = x21;
  x21 = x20;
  x20 = y2;

  if (y1 <= y2)
    return y1 - y2 + M1;
  return y1 - y2;
}

inline unsigned long MRG::sup()
{
  return M1 + 1;
}

inline float MRG::asFloat()
{
  PrivateSingleType result;
  result.s = 1.0;
  result.u |= (asLong() & singleMantissa.u);
  result.s -= 1.0;
#ifdef DEBUG
  assert(result.s < 1.0 && result.s >= 0);
#endif
  return (result.s);
}

inline double MRG::asDouble()
{
  // Original code:
  //  #define NORM 4.656612873077393e-10
  //    return asLong() * NORM;

  PrivateDoubleType result;
  result.d = 1.0;

#if 1 //#cpu(i386) || #cpu(x86_64) FIXME
  unsigned long aux = asLong();
  result.u[1] |= (aux & doubleMantissa.u[1]);
  // Para esta situaci�n construimos un n�mero Long de 32 bits.
  // Ser�a m�s eficiente aprovechar los otros 10 bits de aux no usados.
  result.u[0] |= aux >> 30 ? asLong() : asLong() | 0x80000000;
#else
  // Falta comprobar que se obtiene el mismo n�mero en otra arquitectura.
  unsigned long aux = asLong();
  result.u[0] |= (aux & doubleMantissa.u[0]);
  result.u[1] |= aux >> 30 ? asLong() : asLong() | 0x80000000;
#endif
  result.d -= 1.0;
#ifdef DEBUG
  assert(result.d < 1.0 && result.d >= 0);
#endif
  return (result.d);
}
#endif
