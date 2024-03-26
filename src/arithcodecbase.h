/***************************************************************************
    arithcodecbase.h  -  Definition of class ArithCodecBase
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

/*!
  \file arithcodecbase.h
  \brief Definition of ArithCodecBase class.
  
  This file contains the definition of ArithCodecBase class.
*/


#ifndef ARITHCODECBASE_H
#define ARITHCODECBASE_H



#include "defs.h"
#include "arithdefs.h"
#include "iodevice.h"


/*!
  \brief Abstract predecessor to all arithmetic coding classes.

  Inherit this class whenever you want to create a new arithmetic coding class.
 */
class ArithCodecBase
{
public:
  //! Set the output device for arithmetic coder/decoder.
  virtual void setOutputDevice(IODevice *device) { outputDevice = device; }

  //! Set the input device for arithmetic coder/decoder.
  virtual void setInputDevice(IODevice *device) { inputDevice = device; }

  //! Main encoding routine.
  virtual void arithmeticEncode(FreqValue, FreqValue, FreqValue) = 0;

  //! Decode the target value using the current total frequency and the coder's state variables.
  virtual FreqValue arithmeticDecodeTarget(FreqValue) = 0;

  //! Decode the next input symbol.
  virtual void arithmeticDecode(FreqValue, FreqValue, FreqValue) = 0;

  //! Start the encoder.
  virtual void startEncode(void) = 0;

  //! Finish encoding.
  virtual void finishEncode(void) = 0;

  //! Start the decoder.
  virtual void startDecode(void) = 0;

  //! Finish decoding.
  virtual void finishDecode(void) = 0;

  //! Initialize the bit output function.
  virtual void startOutputtingBits(void) = 0;

  //! Start the bit input function.
  virtual void startInputtingBits(void) = 0;

  //! Complete outputting bits.
  virtual void doneOutputtingBits(void) = 0;

  //! Complete inputting bits.
  virtual void doneInputtingBits(void) = 0;

  //! Return the number of output bytes.
  virtual size_t numberOfOutputBytes(void) = 0;

protected:
  //! Output device.
  IODevice *outputDevice;

  //! Input device.
  IODevice *inputDevice;
}; //ArithCodecBase


#endif //ARITHCODECBASE_H
