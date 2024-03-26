/***************************************************************************
    defs.h  -  System-wide definitions.
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
  \file defs.h
  \brief System-wide definitions.
  
  This file contains definitions of macros and types used throughout the whole system.
*/


#ifndef DEFS_H
#define DEFS_H


#ifdef __WIN32__
# include "winconfig.h"
#else
# ifdef HAVE_CONFIG_H
#  include <config.h>
# endif
#endif

#include <iostream>
#include <iomanip>

#include "debug.h"



//! The default extension of compressed files.
#define DEFAULT_FILE_SUFFIX	".e"



/*!
  \brief The stamp stored in compressed files.

  Exalt uses this stamp to recognize its native file format.
 */
#define FILE_STAMP		"EX"




/*!
  Enable multiplications and divisions. If undefined, shifts and adds will be used.
*/
//#define MULT_DIV 



/*!
  Default code bits.
*/
#define B_BITS		32



/*!
  Default freq bits.
*/
#define F_BITS		27

#if B_BITS < F_BITS + 2
#error : Code bits must be at least freq bits + 2
#endif



/*!
  This option removes redundant bits from the start and end of coding. The standard implementation is wasteful of bits, outputting 1 at the front, and about \a B (eg: 32) at the tail of the coding sequence which it doesn't need to. This option saves those bits, but runs slightly slower.

  Instead of just keeping the difference value, \a V - \a L (code value - low value) in the decoder, frugal bits keeps track of the low value and code value separately, so that only the necessary (1 to 3) disambiguating bits need coding/decoding at the end of coding. Although when the decoder discovers this, \a B_BITS past valid coding output will have been read (possibly past end of file). These bits can be recovered by calling ArithCodec::retrieveExcessInputBits() (for example, if another coding stream was about to start). This saves \a B - 3 to \a B - 1 bits (eg:  With \aB = 32, saving between 29 and 31 bits).  Decoding time is slightly slower, as bitstream window is maintained.

  Another 1 bit is saved by observing that with the initial range of [0, 0.5) which is used, the first bit of each coding sequence is zero. This bit need not be transmitted. This can be implemented efficiently in the decoder, but slows the encoder which checks each time a bit is output whether it is the first bit of a coding sequence. The encoder would not need to be slowed by this if coding always began at the start of a byte, but we are allowing for consecutive coding sequences (ArithCodec::startEncode(); ... ArithCodec::finishEncode(); pairs) in the same bitstream. The status of this option is not stored in the compressed data. Thus it is up to the user to ensure that the FRUGAL_BITS option is the same when decompressing as when compressing.

  \sa ArithCodec::retrieveExcessInputBits(), ArithCodec::startEncode(), ArithCodec::finishEncode()
*/
#define FRUGAL_BITS 1



/*!
  Use sequential algorithm for encoding grammars. (There is no other algorithm implemented yet.)
 */
#define SEQUENTIAL_ALGORITHM





/*!
  \brief An anum of user-selectable actions.

  Values of this enum are used in the main program to indicate what the user wishes to do.
*/
enum UserAction
{
  /*!
    \brief Compress the data.

    Exalt will compress the specified input data. This is a default action.
  */
  Compress,
  
  /*!
    \brief Decompress the data.

    Exalt will decompress the specified input data.
  */
  Decompress,
  
  /*!
    \brief Check XML data well-formedness.

    Exalt will check if the input XML data is well-formed.
  */
  WellFormed,
};



//////////////////// Some output macros ////////////////////



/*!
 \brief Print a string on the standard output.

 Outputs a string on the standard output.

 \param x String.
*/
#define OUTPUT(x)		std::cout << x



/*!
 \brief Print a string and an end of line on the standard output.

 Outputs a string and an end of line on the standard output.

 \param x String.
*/
#define OUTPUTNL(x)		OUTPUT(x << std::endl)



/*!
 \brief Print an end of line on the standard output.

 Outputs an end of line on the standard output.
*/
#define OUTPUTENDLINE		std::cout  << std::endl



/*!
 \brief Print a string on the standard error output.

 Outputs a string on the standard error output.

 \param x String.
*/
#define OUTPUTE(x)		std::cerr << x



/*!
 \brief Print a string and an end of line on the standard error output.

 Outputs a string and an end of line on the standard error output.

 \param x String.
*/
#define OUTPUTENL(x)		OUTPUTE(x << std::endl)



/*!
 \brief Print and end of line on the standard error output.

 Outputs an end of line on the standard output.

 \param x String.
*/
#define OUTPUTEENDLINE		std::cerr  << std::endl



/*!
  \brief Report a warning message.

  Displays a warning message on the standard error output.

  \param x The text of the warning.
*/
#define WRN(x)			std::cerr << "[WARNING] " << x << std::endl



/*!
  \brief Report an error message.

  Displays an error message on the standard error output.

  \param x The text of the error message.
*/
#define ERR(x)			std::cerr << "[ERROR] " << x << std::endl



/*!
  \brief Report a fatal error message.

  Displays a fatal error message on the standard error output, and throws the ExaltFatalErrorException exception.

  \param x The text of the fatal error message.
*/
#define FATAL(x)					\
{							\
  PRINT_SOURCE_POSITION;				\
  std::cerr << "[FATAL ERROR] " << x << std::endl	\
	    << "Aborting." << std::endl;		\
  throw ExaltFatalErrorException();			\
}



/*!
  \brief Report a parser error.

  Displays a parser error message on the standard error output.

  \param x The text of the parser error.
*/
#define PARSERERR(x)		std::cerr << std::endl << "[PARSE ERROR] " << x << std::endl


//! Replacement symbol for ampersands.
#define AMP_REPLACEMENT 6

////////////////////////////////////////////////////////////


/*
  \mainpage Exalt API Documentation
  
  \section intro Introduction
  
  This is the introduction.
  
  \section install Installation
  
  \subsection step1 Step 1: Opening the box
  
  etc...
*/



#endif //DEFS_H
