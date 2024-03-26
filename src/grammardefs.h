/***************************************************************************
    grammardefs.h  -  Definition of macros, types etc. concerning grammars.
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
  \file grammardefs.h
  \brief Definition of macros and types concerning grammars.

  This file contains definitions of types and macros that ere relevant to grammars.
*/

#ifndef GRAMMARDEFS_H
#define GRAMMARDEFS_H


#include "defs.h"
#include "xmldefs.h"


/*!
  \brief Data type representing terminal values.
 */
typedef XmlChar TerminalValue;


/*!
  \brief Enum describing types of symbols in grammar rules.

  Grammar rules consist of two types of symbols: of terminals and variables (also non-terminals). This enum defines constants for these types of symbols.
 */
enum SymbolType
{
  //!Terminal symbol.
  Terminal,

  //!Non-terminal symbol.
  Variable
};



#endif //GRAMMARDEFS_H



