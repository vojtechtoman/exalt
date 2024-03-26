/***************************************************************************
    xmldefs.h  -  XML-specific definitions.
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
  \file xmldefs.h
  \brief XML-specific definitions.
  
  This file contains definitions of some XML-specific types and macros.
*/

#ifndef XMLDEFS_H
#define XMLDEFS_H



//! Whether to print content of SAX events.
//#define PRINT_SAX 1



extern "C"
{
# include <expat.h>
}


#include "defs.h"

/*!
  \brief An alias for handling single XML characters.

  XmlChar is be an alias to XML_Char. Used mainly for convenience.
 */
#define XmlChar XML_Char

/*!
  \brief An alias for Expat's \a XML_Encoding.
 */
#define XmlEncoding XML_Encoding

/*!
  \brief An alias for representing the expat parser structure.

  XmlInnerParser is an alias to XML_Parser. Used mainly for convenience.
 */
#define XmlInnerParser XML_Parser



#ifdef XML_UNICODE
//expat internally uses UTF-8
/*!
  \brief The default output encoding.
*/
# define DEFAULT_OUTPUT_ENCODING		Encodings::UTF_8
#else
//expat internally uses UTF-16
/*!
  \brief The default output encoding.
*/
# define DEFAULT_OUTPUT_ENCODING		Encodings::UTF_16
#endif //XML_UNICODE





//! Predefined entity for character '.
#define ENT_APOS	"&apos;"

//! Predefined entity for character ".
#define ENT_QUOT	"&quot;"

//! Predefined entity for character \&.
#define ENT_AMP		"&amp;"

//! Predefined entity for character \<.
#define ENT_LT		"&lt;"

//! Predefined entity for character \>.
#define ENT_GT		"&gt;"



#endif //XMLDEFS_H



