/***************************************************************************
    exceptions.h  -  Definitions of used exception types.
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
  \file exceptions.h
  \brief Definitions of some used exceptions.
  
  This file contains definitions of exceptions used within the program.
*/

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H


#include "defs.h"


/*!
  \brief Generic Exalt exception.

  Predecessor of all Exalt exceptions, can be used for generic Exalt exceptions handling.
 */
class ExaltException {};


/*!
  \brief NULL pointer operation exception.

  This exception is emitted whenever one tries to DELETE() a NULL pointer, or calls CHECK_POINTER() to NULL pointer.
 */
class ExaltNullPointerException : public ExaltException {};


/*!
  \brief Fatal error exception.

  This exception is raised whenever a fatal error occurs.
 */
class ExaltFatalErrorException : public ExaltException {};

//! Exception reporting wrong program options.
class ExaltOptionException : public ExaltException {};


////////////////////


//! A generic (de)compression exception.
class ExaltCompressionException : public ExaltException{};

//! Exception reporting wrong version of the program.
class ExaltUnknownFileFormatException : public ExaltCompressionException {};

//! Attempt to use uninitialized context.
class ExaltContextNotInitializedException : public ExaltCompressionException {};

//! Different F_BITS or B_BITS.
class ExaltCodeBitsException : public ExaltCompressionException {};

//! An attempt to use a PULL coder in PUSH mode.
class ExaltCoderIsPullException : public ExaltCompressionException {};

//! An attempt to use a PUSH coder in PULL mode.
class ExaltCoderIsPushException : public ExaltCompressionException {};

//! An attempt to use unitialized PUSH coder.
class ExaltPushCoderNotInitializedException : public ExaltCompressionException {};

////////////////////


//! A generic IO exception.
class ExaltIOException : public ExaltException {};

//! Error during IO device preparation.
class ExaltPrepareDeviceIOException : public ExaltIOException {};

//! Error during IO device flushing.
class ExaltFlushDeviceIOException : public ExaltIOException {};

//! Error during IO device finishing.
class ExaltFinishDeviceIOException : public ExaltIOException {};

//! Attempt to use of IO device that is not prepared.
class ExaltDeviceNotPreparedIOException : public ExaltIOException {};

//! IO device is full (e.g. no more data can be written to it).
class ExaltDeviceFullIOException : public ExaltIOException {};

//! An error during read operation.
class ExaltDeviceReadIOException : public ExaltIOException {};

//! An error during write operation.
class ExaltDeviceWriteIOException : public ExaltIOException {};


////////////////////


//! A generic character encoding exception
class ExaltEncodingException : public ExaltException {};

//! An error during character conversion.
class ExaltCharacterConversionException : public ExaltEncodingException {};

//! An unknown encoding occurred.
class ExaltUnknownEncodingException : public ExaltEncodingException {};

//! An unknown encoding occurred.
class ExaltUnsupportedOutputEncodingException : public ExaltEncodingException {};


////////////////////


//! A generic parsing exception.
class ExaltParserException : public ExaltException {};

//! A parse error exception.
class ExaltParseErrorException : public ExaltParserException {};


#endif //EXCEPTIONS_H



