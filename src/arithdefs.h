/***************************************************************************
    arithdefs.h  -  Definitions for arithmetic coding.
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
  \file arithdefs.h
  \brief Definitions for arithmetic coding.
  
  This file contains definitions for arithmetic coding.
*/

#ifndef ARITHDEFS_H
#define ARITHDEFS_H



#include "defs.h"



// *** USER ADJUSTABLE PARAMETERS ***



/*!
  B_BITS of precision.

  Change these types for different precision calculations. They may affect the speed of the arithmetic operations (multiplcation, division, shift, etc).
*/
typedef unsigned long CodeValue;


/*!
  Data type for handling symbol frequencies. The way the Context class is implemented, the type of FreqValue must be able to accomodate F_BITS+1 bits, instead of F_BITS, to avoid overflows. Ie: For an F_BITS of up to 31, type FreqValue must be 32 bits.

  Change this types for different precision calculations. It may affect the speed of the arithmetic operations (multiplcation, division, shift, etc).
 */
typedef unsigned long FreqValue;


/*!
  B_BITS-F_BITS of precision.

Change these types for different precision calculations. They may affect the speed of the arithmetic operations (multiplcation, division, shift, etc).
 */
typedef unsigned long DivValue;



/*!
  A bound on outBitsOutstanding (see ArithCodec). If outBitsOutstanding ever gets above this number (extremely unlikely) the program will abort with an error message.

 \sa ArithCodec::outBitsOutstanding
 */
#define MAX_BITS_OUTSTANDING	((unsigned long)1<<31)



// *** END USER ADJUSTABLE PARAMETERS ***


#endif //ARITHDEFS_H
