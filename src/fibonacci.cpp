/***************************************************************************
    fibonacci.cpp  -  Definition of Fibonacci class methods.
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
  \file fibonacci.cpp
  \brief Definition of Fibonacci class methods.

  This file contains definition of Fibonacci class methods.
*/


#include "fibonacci.h"


#ifdef __GNUG__
# pragma implementation
#endif


//  Generated list of Fibonacci numbers of order 4 (first number omitted).
//  Unused.
//  static unsigned long Order4FibonacciNumbers_[] = {
//    /*1,*/ 1, 4, 7, 13,
//    25, 49, 94, 181, 349,
//    673, 1297, 2500, 4819, 9289,
//    17905, 34513, 66526, 128233, 247177,
//    476449, 918385, 1770244, 3412255, 6577333,
//    12678217, 24438049, 47105854, 90799453, 175021573,
//    337364929, 650291809
//  };

//  Number of generated Fibonacci numbers of order 4.
//  #define NUMBER_OF_ORDER4_FIBS 		31


//! Generated list of Fibonacci numbers of order 2 (first number omitted).
static unsigned long Order2FibonacciNumbers_[] = {
  1, 2, 3, 5,
  8, 13, 21, 34, 55,
  89, 144, 233, 377, 610,
  987, 1597, 2584, 4181, 6765,
  10946, 17711, 28657, 46368, 75025,
  121393, 196418, 317811, 514229, 832040,
  1346269, 2178309, 3524578, 5702887, 9227465,
  14930352, 24157817, 39088169, 63245986, 102334155,
  165580141, 267914296, 433494437, 701408733, 1134903170
};

//! Number of generated Fibonacci numbers of order 2.
#define NUMBER_OF_ORDER2_FIBS 		44


/*!
  Constructs a Fibonacci code for given \a number. The code is stored reversely (eg. as 11000 instead of 00011) which allows easier manipulation using shifts.

  \param number Coded integer number (greater than 0).
  \return Fibonacci code.

  \sa decode(), encodeToBuffer().
 */
unsigned long Fibonacci::encode(unsigned long number)
{
  int i = NUMBER_OF_ORDER2_FIBS - 1;

  unsigned long fn;
  unsigned long code;
  
  code = 1;

  while (i > -1)
    {
      fn = Order2FibonacciNumbers_[i];

      if (fn <= number)
	{
	  number -= fn;
	  
	  code <<= 1;
	  code |= 1;
	}
      else
	if (code > 1)
	  code <<= 1;

      i--;
    }

  return code;
}



/*!
  Reconstructs a number from given Fibonacci \a code. The code must be reversed.

  \param code Reversed Fibonacci code.
  \return Reconstructed number.

  \sa encode(), decodeFromBuffer().
 */
unsigned long Fibonacci::decode(unsigned long code)
{
  unsigned long number = 0;
  int i = 0;
  bool lastWasOne = false;
  
  while (code)
    {
      if (code & 1)
	{
	  if (!lastWasOne)
	    {
	      number += Order2FibonacciNumbers_[i];

	      lastWasOne = true;
	    }
	  else
	    //two 11 appeared --> code ended
	    return number;
	}
      else
	lastWasOne = false;
      
      code >>= 1;
      i++;
    }

  return number;
}



/*!
  Constructs Fibonacci code for given \a number and stores it in the \a buffer. The \a itemSize parameter indicates the size of one buffer item in bytes (or more precisely, in chars).

  \warning No checks are performed, so memory error can occur in case of too short buffer!

  \param buffer Pointer to the buffer.
  \param itemSize Size of one item of the buffer (in bytes).
  \param number Encoded number.

  \return Number of buffer items which contain the code.

  \sa encode(), decodeFromBuffer().
 */
size_t Fibonacci::encodeToBuffer(void *buffer, size_t itemSize, unsigned long number)
{
  unsigned char *charBuf = (unsigned char *)buffer;
  unsigned char tmpChar = 0;
  unsigned char bits = 0;
  size_t charsTotal = 0;
  unsigned long code = encode(number);


  while (code)
    {
      tmpChar <<= 1;
      tmpChar |= (code & 1);
      bits++;

      if (bits == SIZEOF_CHAR*8)
	{
	  //store char to the buffer
	  charBuf[charsTotal] = tmpChar;
	  charsTotal++;
	  bits = 0;
	  tmpChar = 0;
	}

      code >>= 1;
    }

  if (!bits)
    //exactly n bytes
    charsTotal--;
  else
    {
      //output the last "partial" char
      //"align it to the left"
      while (bits < SIZEOF_CHAR*8)
      {
	tmpChar <<= 1;
	bits++;
      }
      charBuf[charsTotal] = tmpChar;
    }


  while ((charsTotal + 1) % itemSize)
    {
      //add fill chars if needed
      charsTotal++;
      charBuf[charsTotal] = 0;
    }

  return (charsTotal+1) / itemSize;
}




/*!
  Decodes Fibonacci \a buffer and returns the decoded number.

  \warning No checks are performed, so memory error can occur in case of too short buffer!

  \param buffer Pointer to the buffer.
  \param itemSize Size of one item of the buffer (in bytes).
  \param nrItems Number of buffer items used during decoding.

  \return Decoded number.

  \sa decode(), encodeToBuffer().
 */
size_t Fibonacci::decodeFromBuffer(void *buffer, size_t itemSize, size_t *nrItems)
{
  unsigned long code = 0;
  unsigned long number;
  unsigned char *charBuf = (unsigned char *)buffer;
  bool lastWasOne = false;
  unsigned char tmpChar = 0;
  size_t charsTotal = 0;
  unsigned char mask;


  while (true)
    {
      tmpChar = charBuf[charsTotal];
      charsTotal++;
      mask = 1 << (SIZEOF_CHAR*8-1);

      while (mask)
	{
	  if (tmpChar & mask)
	    {
	      if (!lastWasOne)
		{
		  code <<= 1;
		  code |= 1;
		  lastWasOne = true;
		}
	      else
		{
		  number = decode(code);

		  while (charsTotal % itemSize)
		    charsTotal++;
		  
		  *nrItems = charsTotal / itemSize;
		  return number;
		}
	    }
	  else
	    {
	      code <<= 1;
	      lastWasOne = false;
	    }

	  mask >>= 1;
	}
    }

}


