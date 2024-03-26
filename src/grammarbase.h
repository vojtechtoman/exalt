/***************************************************************************
    grammarbase.h  -  Definition of GrammarBase class.
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
  \file grammarbase.h
  \brief Definition of GrammarBase class.

  This file contains the definition of GrammarBase class which represents an abstract prototype of a grammar.
*/

#ifndef GRAMMARBASE_H
#define GRAMMARBASE_H


#include "defs.h"
#include "options.h"
#include "grammardefs.h"
#include "contextbase.h"
#include "iodevice.h"
#include "textcodec.h"

/*!
  \brief Abstract prototype of a grammar.

  This class represents an abstract prototype for all grammars.
 */
class GrammarBase : public UserOfTextCodec
{
public:
  //! A constructor.
  GrammarBase(void) : UserOfTextCodec() {}


  /*!
    \brief A destructor.

    Does nothing.
   */
  virtual ~GrammarBase(void) {}


  /*!
    \brief Set the base size of terminal alphabet.
    \param size Size of the alphabet.

    Call this method if you want to override a default size (256) of the terminal alphabet.
   */
  virtual void setAlphabetBaseSize(size_t size) = 0;
  /*!
    \brief Append one terminal symbol to the root rule of the grammar.
    \param value Appended terminal symbol.

    This method is used for appending one terminal symbol to the root rule of the grammar.
   */
  virtual void append(TerminalValue value) = 0;


  /*!
    \brief Ensure that all appended terminal symbols have been processed.
    \sa append()

    Appending a terminal symbol by append() doesn't necessarily mean that this symbol will be immediately processed. Depending on its implementation, the grammar sometimes can (for whatever reason) wait for larger number of terminal symbols. Call to flush() ensures that all appended symbols are processed.
   */
  virtual void flush(void) = 0;


  /*!
    \brief Print contents of the grammar.

    This method outputs a human-readable representation of the grammar on the standard output.
   */
  virtual void print(void) = 0;


  /*!
    \brief Reconstruct data represented by the grammar.

    Call to this method results in reconstruction of the data represented by the grammar.
   */
  virtual void reconstructInput(void) = 0;


};


#endif //GRAMMARBASE_H



