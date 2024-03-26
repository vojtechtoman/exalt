/***************************************************************************
    contextbase.h  -  Definition of ContextBase class.
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
  \file contextbase.h
  \brief Definition of ContextBase class.
  
  This file contains the definition of ContextBase virtual class.
*/

#ifndef CONTEXTBASE_H
#define CONTEXTBASE_H


#include "defs.h"
#include "arithcodec.h"



/*!
  \brief Type of the context.

  Context can be of two types: static and dynamic.
 */
enum ContextType
{
  //! Context cannot grow (no escape needed).
  StaticContext,

  //! Context can grow (escape needed).
  DynamicContext
};






/*!
  \brief Abstract predecessor for all context classes used in arithmetic coding/decoding.
*/
class ContextBase
{
public:
  //! A constructor.
  ContextBase(void) { initialized = false; }

  //! An enum with results of some contex operations.
  enum OperationResult
  {
    //! Everything was ok.
    Ok = 0,
    
    //! Attempt to code unknown symbol.
    NotKnown = -1,
    
    //! Could not install symbol.
    TooManySymbols = -2,
    
    //! Memory exceeded during installing symbol.
    NoMemory = -3
  };

  //! The "end of message" symbol.
  int endOfMessage;


  //! Initialization of the context with given size and type.
  virtual void setType(int, ContextType) = 0;

  //! Initialize context.
  virtual int initialize(void) = 0;

  //! Return the value of last fixed symbol in the context.
  virtual int lastFixedSymbol(void) = 0;

  //! Initialize context.
  virtual bool isInitialized(void) { return initialized; }

  //! Set arithmetic coder for the context.
  virtual void setArithCodec(ArithCodec *ac) { arithCodec = ac; }

  //! Encode symbol using given arithmetic coder/decoder.
  virtual int encode(int) throw (ExaltContextNotInitializedException, ExaltIOException) = 0;

  //! Encode the "end of message" symbol.
  virtual int encodeEndOfMessage() throw (ExaltContextNotInitializedException, ExaltIOException)= 0;

  //! Decode symbol using given arithmetic coder/decoder.
  virtual int decode(void) throw (ExaltContextNotInitializedException, ExaltIOException) = 0;

  //! Install new symbol.
  virtual int installSymbol(int) = 0;

  //! Purge the context
  virtual void purgeContext(void) = 0;

protected:
  //! Pointer to arithmetic codec.
  ArithCodec *arithCodec;

  //! Flag whether the context is initialized.
  bool initialized;
}; //ContextBase


#endif //CONTEXTBASE_H
