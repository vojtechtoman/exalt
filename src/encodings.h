/***************************************************************************
    encodings.h  -  Definition of macros, types etc. relative to character encodings.
                             -------------------
    begin                : October 31 2002
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
  \file encodings.h
  \brief Definition of macros and types relative to character encodings.

  This file contains definitions of types and macros that are relevant to character encodings.
*/


#ifndef ENCODINGS_H
#define ENCODINGS_H


#include "defs.h"


//! A namespace for character encodings.
namespace Encodings
{
  /*!
    \brief An enum of some "recognized" character encodings.
    
    Every item of the enum is assigned a MIB code (see http://www.iana.org/assignments/charset-reg/). Exalt provides a framework for character sets conversions (see TextCodec). If you want to extend the character sets support, reimplement TextCodec.
  */
  enum MIB
  {
    Unknown	=	-1,
    UTF_8	=	106,
    UTF_16	=	1015,
    US_ASCII	=	3,
    ISO_8859_1	=	4,
    ISO_8859_2	=	5,
    ISO_8859_3	=	6,
    ISO_8859_4	=	7,
    ISO_8859_5	=	8,
    ISO_8859_6	=	82,
    ISO_8859_7	=	10,
    ISO_8859_8	=	85,
    ISO_8859_9	=	12,
    ISO_8859_10	=	13,
    ISO_8859_11	=	2259,	//Thai - MIB taken from tis620 (which is byte by byte equivalent)
    ISO_8859_13	=	109,
    ISO_8859_14	=	110,
    ISO_8859_15	=	111,
    CP_1250	=	2250,
    CP_1251	=	2251,
    CP_1252	=	2252,
    CP_1253	=	2253,
    CP_1254	=	2254,
    CP_1255	=	2255,
    CP_1256	=	2256,
    CP_1257	=	2257,
    CP_1258	=	2258,
    KOI8_R	=	2084,
    KOI8_V	=	2088
  };



  /*!
    \brief The name of one encoding.

    Each encoding is assigned a character name. There is an exact mapping from the encoding;s MIB to its name.
   */
  struct EncodingName
  {
    //! The name of the encoding.
    const char *name;

    //! The MIB of the encoding.
    MIB mib;
  };
}



/*!
  \brief %List of encoding names.

  To be used for the initialization of an encoding names array.
 */
#define ENCODING_NAMES						\
{"UTF-8", Encodings::UTF_8},					\
{"UTF-16", Encodings::UTF_16},					\
{"US-ASCII", Encodings::US_ASCII},				\
{"ISO-8859-1", Encodings::ISO_8859_1},				\
{"ISO-8859-2", Encodings::ISO_8859_2},				\
{"ISO-8859-3", Encodings::ISO_8859_3},				\
{"ISO-8859-4", Encodings::ISO_8859_4},				\
{"ISO-8859-5", Encodings::ISO_8859_5},				\
{"ISO-8859-6", Encodings::ISO_8859_6},				\
{"ISO-8859-7", Encodings::ISO_8859_7},				\
{"ISO-8859-8", Encodings::ISO_8859_8},				\
{"ISO-8859-9", Encodings::ISO_8859_9},				\
{"ISO-8859-10", Encodings::ISO_8859_10},			\
{"ISO-8859-11", Encodings::ISO_8859_11},			\
{"ISO-8859-13", Encodings::ISO_8859_13},			\
{"ISO-8859-14", Encodings::ISO_8859_14},			\
{"ISO-8859-15", Encodings::ISO_8859_15},			\
{"CP-1250", Encodings::CP_1250},				\
{"CP-1251", Encodings::CP_1251},				\
{"CP-1252", Encodings::CP_1252},				\
{"CP-1253", Encodings::CP_1253},				\
{"CP-1254", Encodings::CP_1254},				\
{"CP-1255", Encodings::CP_1255},				\
{"CP-1256", Encodings::CP_1256},				\
{"CP-1257", Encodings::CP_1257},				\
{"CP-1258", Encodings::CP_1258},				\
{"KOI8-R", Encodings::KOI8_R},					\
{"KOI8-V", Encodings::KOI8_V},					\
/* keep this item at the end of the list */			\
{0, Encodings::Unknown}



/*!
  \brief List the names of the encodings.

  Displays a comma-separated list of encoding names.
 */
#define LIST_ENCODINGS						\
{								\
  Encodings::EncodingName encodingNames[] = { ENCODING_NAMES };	\
  size_t i = 0;							\
								\
  while (encodingNames[i].name && encodingNames[i+1].name)	\
    {								\
      OUTPUT(encodingNames[i].name << ", ");			\
      i++;							\
    }								\
  if (encodingNames[i].name)					\
    {								\
      OUTPUTNL(encodingNames[i].name);				\
    }								\
}



#endif //ENCODINGS_H



