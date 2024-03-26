/***************************************************************************
    unroll.h  -  Loop unrolling macros.
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
  \file unroll.h
  \brief Used for loop unrolling.

  Duplicates the given code fragment a specified
  number of times.

  \code
  ---------------------------------------------------------- 
  Precondition:
	#define UNROLL_NUM	Number of repetitions
	#define UNROLL_CODE	Code to repeat
  To use:
	#include "unroll.i"
  Result:
	The code UNROLL_CODE repeated UNROLL_NUM times.
  ---------------------------------------------------------- 

  For example:
        The program fragment:
		...
		i=0;
		#define UNROLL_NUM 3
		#define UNROLL_CODE  printf("%i",i++);
		#include "unroll.i"
		...

	is equivalent to:
		...
		i=0;
		printf("%i",i++);
		printf("%i",i++);
		printf("%i",i++);
		...
  ---------------------------------------------------------- 
  \endcode

  If the number of times the code fragment is to be repeated is greater
  than a certain number (here, 32), then a loop is added to repeat the code
  fragment as required.  
 */


#if UNROLL_NUM > 0
  UNROLL_CODE
#endif
#if UNROLL_NUM > 1
  UNROLL_CODE
#endif
#if UNROLL_NUM > 2
  UNROLL_CODE
#endif
#if UNROLL_NUM > 3
  UNROLL_CODE
#endif
#if UNROLL_NUM > 4
  UNROLL_CODE
#endif
#if UNROLL_NUM > 5
  UNROLL_CODE
#endif
#if UNROLL_NUM > 6
  UNROLL_CODE
#endif
#if UNROLL_NUM > 7
  UNROLL_CODE
#endif
#if UNROLL_NUM > 8
  UNROLL_CODE
#endif
#if UNROLL_NUM > 9
  UNROLL_CODE
#endif
#if UNROLL_NUM > 10
  UNROLL_CODE
#endif
#if UNROLL_NUM > 11
  UNROLL_CODE
#endif
#if UNROLL_NUM > 12
  UNROLL_CODE
#endif
#if UNROLL_NUM > 13
  UNROLL_CODE
#endif
#if UNROLL_NUM > 14
  UNROLL_CODE
#endif
#if UNROLL_NUM > 15
  UNROLL_CODE
#endif
#if UNROLL_NUM > 16
  UNROLL_CODE
#endif
#if UNROLL_NUM > 17
  UNROLL_CODE
#endif
#if UNROLL_NUM > 18
  UNROLL_CODE
#endif
#if UNROLL_NUM > 19
  UNROLL_CODE
#endif
#if UNROLL_NUM > 20
  UNROLL_CODE
#endif
#if UNROLL_NUM > 21
  UNROLL_CODE
#endif
#if UNROLL_NUM > 22
  UNROLL_CODE
#endif
#if UNROLL_NUM > 23
  UNROLL_CODE
#endif
#if UNROLL_NUM > 24
  UNROLL_CODE
#endif
#if UNROLL_NUM > 25
  UNROLL_CODE
#endif
#if UNROLL_NUM > 26
  UNROLL_CODE
#endif
#if UNROLL_NUM > 27
  UNROLL_CODE
#endif
#if UNROLL_NUM > 28
  UNROLL_CODE
#endif
#if UNROLL_NUM > 29
  UNROLL_CODE
#endif
#if UNROLL_NUM > 30
  UNROLL_CODE
#endif
#if UNROLL_NUM > 31
  UNROLL_CODE
#endif

#if UNROLL_NUM > 32
  {  int __i7;
    for (__i7=0; __i7 < UNROLL_NUM-32; __i7++)
      {
	UNROLL_CODE
      }
  }
#endif

#undef UNROLL_NUM
#undef UNROLL_CODE
