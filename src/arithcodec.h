/***************************************************************************
    arithcodec.h  -  Definition of class ArithCodec
                             -------------------
    begin                : June 21 2002
    copyright            : (C) 2003 by Vojtìch Toman
    email                : vtoman@lit.cz
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/* Based on code by:                                                       */


/***************************************************************************
Authors: 	John Carpinelli   (johnfc@ecr.mu.oz.au)
	 	Wayne Salamonsen  (wbs@mundil.cs.mu.oz.au)
		Lang Stuiver	  (langs@cs.mu.oz.au)

Purpose:	Data compression using revised arithmetic coding method.

Based on: 	A. Moffat, R. Neal, I.H. Witten, "Arithmetic Coding Revisted",
		Proc. IEEE Data Compression Conference, Snowbird, Utah, 
		March 1995.

Copyright 1995 John Carpinelli and Wayne Salamonsen, All Rights Reserved.
Copyright 1996 Lang Stuiver, All Rights Reserved.

These programs are supplied free of charge for research purposes only,
and may not sold or incorporated into any commercial product.  There is
ABSOLUTELY NO WARRANTY of any sort, nor any undertaking that they are
fit for ANY PURPOSE WHATSOEVER.  Use them at your own risk.  If you do
happen to find a bug, or have modifications to suggest, please report
the same to Alistair Moffat, alistair@cs.mu.oz.au.  The copyright
notice above and this statement of conditions must remain an integral
part of each and every copy made of these files.
***************************************************************************/

/*!
  \file arithcodec.h
  \brief Definition of ArithCodec class.
  
  This file contains the definition of ArithCodec class.
*/


#ifdef __GNUG__
#pragma interface
#endif

#ifndef ARITHCODEC_H
#define ARITHCODEC_H


#include "defs.h"
#include "arithcodecbase.h"



//! Size of one byte.
#define BYTE_SIZE	SIZEOF_CHAR*8


//! Half of the interval.
#define Half		((CodeValue) 1 << (B_BITS-1))


//! Quarter of the interval.
#define Quarter		((CodeValue) 1 << (B_BITS-2))


/*!
  \brief Arithmetic coder/decoder.

  The coding routines implemented by this class contain two different methods for doing the required arithmetic. In defs.h you must specify whether to use multiplication and division, or shifts and adds. You can also specify the number of frequency and code bits (F_BITS and B_BITS, respectively) at compile time. Fixing them at compile time gives improved performance, especially for the shift/add version.

  Fixing bits:
  Usually frequency bits and code bits would only be changed for testing, and in a standard implementation might well be fixed at compile time. The advantage here is that the shift/add loop can be unrolled, without need for testing and decrementing a counter variable. The loops are unrolled explicitly by calling an include file unroll.i. The compiler may well be able to unroll the loops automatically when they are written to be detectable (with \p -funroll-loops in gcc, for example). The aim here was to emphasise what is going on at the machine level, and to ensure similar performance was obtained independent of the C compiler used.

  Difference from conventional arithmetic coding: Approximate arithmetic allows a greater frequency range, at expense of compression size. The inefficiency is bounded by the maximum frequency and when B_BITS = 32 and F_BITS = 27, for example, this inefficency is negligible. If the shift/add optimisations are used, the total frequency count, t, must be partially normalised, so that 2^{f - 1} < t <= 2^f, where f is frequency bits, the variable "F_BITS". This partial normalisation applies only to shift/add arithmetic; the original multiplication/division arithmetic will still work correctly with arbitrary values for t).

  FRUGAL_BITS option: If you \#define FRUGAL_BITS, some bits in the output wil be saved.
*/
class ArithCodec : public ArithCodecBase
{
public:
  //! A constructor.
  ArithCodec(void);

  //! Set the output device for arithmetic coder/decoder.
  //virtual void setOutputDevice(IODevice *device);

  //! Set the input device for arithmetic coder/decoder.
  //virtual void setInputDevice(IODevice *device);

  //! Main encoding routine.
  virtual void arithmeticEncode(FreqValue, FreqValue, FreqValue);

  //! Decode the target value using the current total frequency and the coder's state variables.
  virtual FreqValue arithmeticDecodeTarget(FreqValue);

  //! Decode the next input symbol.
  virtual void arithmeticDecode(FreqValue, FreqValue, FreqValue);

  //void binaryArithmeticEncode(FreqValue, FreqValue, int);
  //int binaryArithmeticDecode(FreqValue, FreqValue);

  //! Start the encoder.
  virtual void startEncode(void);

  //! Finish encoding.
  virtual void finishEncode(void);

  //! Start the decoder.
  virtual void startDecode(void);

  //! Finish decoding.
  virtual void finishDecode(void);

  //! Initialize the bit output function.
  virtual void startOutputtingBits(void);

  //! Start the bit input function.
  virtual void startInputtingBits(void);

  //! Complete outputting bits.
  virtual void doneOutputtingBits(void);

  //! Complete inputting bits.
  virtual void doneInputtingBits(void);

  //! Return the number of output bytes.
  virtual size_t numberOfOutputBytes(void);

protected:
  // Input decoding state

  //! Code range.
  CodeValue in_R;

  //! V - L (V offset).
  CodeValue in_D;

  //! Normalized range.
  DivValue in_r;

#ifdef FRUGAL_BITS
  //! Bitsream window.
  CodeValue in_V;
#endif
  
  // Output encoding state

  //! Number of input/output bytes.
  long numberOfBytes;

  //! Lower bound.
  CodeValue out_L;

  //! Code range.
  CodeValue out_R;

  //! Follow bit count.
  unsigned long	outBitsOutstanding;

#ifdef FRUGAL_BITS
  //! Flag whether to ignore first bit.
  int ignoreFirstBit;

  //! Flag whether reading first message.
  int firstMessage;

  //! Read B_BITS beyond valid output (with FRUGAL_BITS).
  CodeValue retrieveExcessInputBits(void);
#endif

  //! Input buffer.
  int inBuffer;

  //! Input bit pointer.
  unsigned char inBitPtr;

  //! NUmber of bytes read past EOF.
  int inGarbage;
  
  //! Output buffer.
  int outBuffer;

  //! Output bits in buffer.
  int outBitsToGo;

}; //ArithCodec


#endif //ARITHCODEC_H
