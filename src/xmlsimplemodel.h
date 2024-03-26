/***************************************************************************
    xmlsimplemodel.h  -  Definition of XmlSimpleModel class
                             -------------------
    begin                : October 3 2002
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
  \file xmlsimplemodel.h
  \brief Definition of XmlSimpleModel class.

  This file contains definition of XmlSimpleModel class.
*/


#ifdef __GNUG__
# pragma interface
#endif


#ifndef XMLSIMPLEMODEL_H
#define XMLSIMPLEMODEL_H



#include "defs.h"
#include "fibonacci.h"
#include "funneldevice.h"
#include "hashtable.h"
#include "list.h"
#include "queue.h"
#include "saxemitter.h"
#include "stack.h"
#include "textcodec.h"
#include "xmlchar.h"
#include "xmlmodelbase.h"
#include "options.h"


//! A namespace for structural symbols.
namespace StructuralSymbols
{
  //! Enumeration of used structural symbols.
  enum Symbol
  {
    //! No symbol.
    None = -1,

    //! End of data block.
    EndOfBlock = 0,

    //! Start of new (unknown) element.
    NewElement = 1,

    //! Start of known element.
    KnownElement = 2,

    //! End of element.
    EndElement = 3,


    //! Comment data.
    Comment = 4,

    //! Processing instruction.
    PI = 5,

    //! Document type declaration.
    Doctype = 6,

    //! XML declaration.
    XmlDecl = 7,

    //! Start or end of CDATA section.
    CDATA = 8,

    //! Start of the attribute section.
    Attributes = 11,
    
    //! The entity declaration.
    EntityDecl = 14,

    //! The notation declaration.
    NotationDecl = 15,

    //! The default data.
    Default = 16,

    //! The document is standalone.
    StandaloneYes = 1,

    //! The document is not standalone.
    StandaloneNo = 2,

    //! The "standalone" flag was not specified.
    StandaloneNotSpecified = 0,

    //! The DTD has an internal subset.
    HasInternalSubsetYes = 1,

    //! The DTD hasn't an internal subset.
    HasInternalSubsetNo = 2,

    //! The entity is a parameter entity.
    IsParameterEntityYes = 1,

    //! The entity is not a parameter entity.
    IsParameterEntityNo = 2,

    //! The empty string.
    EmptyString = 18,

  };
}


/*!
  \brief Structure representing one element context.
 */
struct SimpleElementContext
{
  //! The name of the element.
  XmlChar *name;
};


//! Hashtable of element contexts (keys are element ids).
typedef HashTable<unsigned long, SimpleElementContext, List, 3079> SimpleElementTable;



/*!
  \brief A simple XML modeling class.

  This class implements a simple model for compression and decompression of XML data.
 */
class XmlSimpleModel : public XmlModelBase
{
public:
  //! A constructor
  XmlSimpleModel(void);
  
  //! A destructor.
  virtual ~XmlSimpleModel(void);

  //! Manage XML events.
  virtual bool manageEvent(XmlModelEvent *event);

  /*!
    \brief Set a grammar for the model.

    Sets a grammar that is used for the compression of the data.

    \param gr The pointer to the grammar.
  */
  virtual void setGrammar(GrammarBase *gr) { grammar = gr; }

  //! Set SAX emitter for decoding.
  virtual void setSAXEmitter(SAXEmitter *emitter, void *data = 0);

  //! Receive data from another object.
  virtual void receiveData(XmlChar *data, size_t size);


protected:
  //! The grammar used for compression.
  GrammarBase *grammar;


  //! Indication of CDATA section.
  bool inCDATA;

  //! Indication of DOCTYPE section.
  bool inDoctype;

  //! Indication whether current element has any attributes.
  bool elementHasAttributes;

  //! Flag whether all the attributes have been decoded.
  bool attributeListComplete;

  /*!
    \brief State of the model.

    Used during decoding. State is represented by StructuralSymbols.
  */
  StructuralSymbols::Symbol state;

  //! Hashtable of element ids.
  ElementIds *elementIds;

  //! Hashtable of element contexts.
  SimpleElementTable *elements;

  //! %List of element names.
  ElementNamesList *elementNamesList;

  //! Counter for unique element numbering.
  unsigned long elementCounter;

  /*!
    \brief Stack of element ids.

    Stores the "level" in the document.
  */
  Stack<unsigned long> *elementStack;

  /*!
    \brief Buffer for read data.

    Used during decoding.
  */
  XmlChar *buffer;

  //! Length of the data in the buffer.
  size_t bufferLength;

  //some variables for working with Fibonacci codes

  /*! 
    \brief Number of read bits of Fibonacci code.
    
    Used during decoding.
  */
  size_t fibBits;

  /*!
    \brief Read Fibonacci code.
    
    Used during decoding.
  */
  unsigned long fibCode;

  //! Helper variable for decoding Fibonacci codes.
  bool fibLastWasOne;

  //! The queue of decoded data items (such as names of the attributes, etc.)
  DataQueue *dataQueue;

  /*!
    \brief Pointer to the SAX emitter.

    Used during decoding.
  */
  SAXEmitter *saxEmitter;

  /*!
    \brief User data for SAX emitter.

    Used during decoding.
   */
  void *userData;
};


#endif //XMLSIMPLEMODEL_H
