/***************************************************************************
    fibonacci.h  -  Definition of Fibonacci class.
                             -------------------
    begin                : October 23 2002
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

/*!
  \file fibonacci.h
  \brief Definition of Fibonacci class.

  This file contains definition of Fibonacci class.
*/

#ifdef __GNUG__
# pragma interface
#endif


#ifndef FIBONACCI_H
#define FIBONACCI_H





#include <cstdlib>	//for size_t


#include "defs.h"
#include "iodevice.h"

/*!
  \brief Class for representing integer numbers with Fibonacci codes.

  This class provides some basic functionality for manipulating with order 2 Fibonacci numbers. It can encode and decode integers (greater than 0). In addition, it is capable of encoding numbers into data buffer, and also decoding this buffers.
 */
class Fibonacci
{
public:
  //! Encode number to Fibonacci code.
  static unsigned long encode(unsigned long number);

  //! Decode number from Fibonacci code.
  static unsigned long decode(unsigned long code);

  //! Encode number to given buffer.
  static size_t encodeToBuffer(void *buffer, size_t itemSize, unsigned long number);

  //! Decode given Fibonacci buffer.
  static size_t decodeFromBuffer(void *buffer, size_t itemSize, size_t *nrItems);

}; //of Fibonacci

#endif //FIBONACCI_H



