/***************************************************************************
    context.h  -  Definition of Context class.
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
Authors: 	John Carpenelli   (johnfc@ecr.mu.oz.au)
	 	Wayne Salamonsen  (wbs@mundil.cs.mu.oz.au)

Purpose:	Data compression using a word-based model and revised 
		arithmetic coding method.

Based on: 	A. Moffat, R. Neal, I.H. Witten, "Arithmetic Coding Revisited",
		Proc. IEEE Data Compression Conference, Snowbird, Utah, 
		March 1995.


Copyright 1995 John Carpinelli and Wayne Salamonsen, All Rights Reserved.

These programs are supplied free of charge for research purposes only,
and may not sold or incorporated into any commercial product.  There is
ABSOLUTELY NO WARRANTY of any sort, nor any undertaking that they are
fit for ANY PURPOSE WHATSOEVER.  Use them at your own risk.  If you do
happen to find a bug, or have modifications to suggest, please report
the same to Alistair Moffat, alistair@cs.mu.oz.au.  The copyright
notice above and this statement of conditions must remain an integral
part of each and every copy made of these files.
****************************************************************************/


/*!
  \file context.h
  \brief Definition of Context class.
  
  This file contains the definition of Context class.
*/


#ifndef CONTEXT_H
#define CONTEXT_H


#ifdef __GNUG__
# pragma interface
#endif


#include <cstdlib>		//for free() and malloc()


#include "defs.h"
#include "contextbase.h"



//! Length of character contexts.
//#define CHAR_CONTEXT_LENGTH	256


//! Add the end '1' bit of a binary number using two's complement arithmetic.
#define BACK(i)			((i) & ((i) - 1))

//! Remove the end '1' bit of a binary number using two's complement arithmetic.
#define FORW(i)			((i) + ((i) & - (i)))


//! Maximum frequency value.
#define MAX_FREQUENCY		((FreqValue) 1 << F_BITS)


//! Minimum increment value.
#define MIN_INCR		1



/*!
  \briefPut most probable symbol at end of range (for more accurate approximations).

  This option influences the compressed data, but is not recorded in the data stream. It is for experimental purposes and it is recommended that it is left \#defined.
 */
#define MOST_PROB_AT_END	1








/*!
  \brief Context structure for arithmetic coding.

  Class represents a frequency table using a Fenwick's binary index tree. Table may be static or dynamic. Dynamic tables may grow above their intial length as new symbols are installed.
 */
class Context : public ContextBase
{
public:
  //! A constructor.
  Context(void);

  //! A constructor. Sets the size and type directly.
  Context(int, ContextType);

  //! A destructor.
  virtual ~Context(void);

  //! Initialization of the context with given size and type.
  virtual void setType(int, ContextType);

  //! Initialize the context.
  virtual int initialize(void);

  //! Return the value of last fixed symbol in the context.
  virtual int lastFixedSymbol(void);

  //! Encode symbol using given arithmetic codec.
  virtual int encode(int) throw (ExaltContextNotInitializedException, ExaltIOException);

  //! Encode the "end of message" symbol.
  virtual int encodeEndOfMessage() throw (ExaltContextNotInitializedException, ExaltIOException);

  //! Decode symbol using given arithmetic codec.
  virtual int decode(void) throw (ExaltContextNotInitializedException, ExaltIOException);

  //! Purge the contents of the context.
  virtual void purgeContext(void);

  //! Install new symbol.
  virtual int installSymbol(int);

protected:
  //! Original size of context.
  int initialSize;

  //! Length of the tree and current length.
  int maxLength, length;

  //! Number of symbols with frequency = 1.
  FreqValue nSingletons;

  //! Type of the context.
  ContextType type;

  //! Count of installed symbols.
  int nSymbols;

  //! Total sum of all frequencies.
  FreqValue total;

  //! Fenwick's binary index tree.
  FreqValue *tree;

  //! Current increment.
  FreqValue incr;

  //! Symbol with highest frequency.
  int mostFreqSymbol;

  //! The highest frequency.
  FreqValue mostFreqCount;

  //! Position of the most frequent symbol.
  FreqValue mostFreqPos;


  //! Get interval for given symbol.
  virtual void getInterval(FreqValue *, FreqValue *, int);

  //! Halve frequencies in the context.
  virtual void halveContext(void);

  //! Initialize number of singletons.
  virtual void initZeroFreq();

}; //Context


#endif //CONTEXT_H
