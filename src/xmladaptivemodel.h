/***************************************************************************
    xmladaptivemodel.h  -  Definition of XmlAdaptiveModel class
                             -------------------
    begin                : November 18 2002
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
  \file xmladaptivemodel.h
  \brief Definition of XmlAdaptiveModel class.

  This file contains definition of XmlAdaptiveModel class.
*/


#ifdef __GNUG__
# pragma interface
#endif


#ifndef XMLADAPTIVEMODEL_H
#define XMLADAPTIVEMODEL_H



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
#include "elementmodel.h"
#include "options.h"


/*!
  \brief A namespace of adaptive structural symbols.

  The adaptive structural symbols are used by XmlAdaptiveModel to mark-up the XML structure in the compressed stream. Adaptive structural symbols occupy smaller range of values than simple structural symbols, because of the more intelligent behaviour of the adaptive model.

  \sa StructuralSymbols
 */
namespace AdaptiveStructuralSymbols
{
  //! An enumeration of used structural symbols.
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

    Characters = 4,

    //! Comment data.
    Comment = 1,

    //! Processing instruction.
    PI = 2,

    //! Start or end of CDATA section.
    CDATA = 3,

    //! The violation of an assumption.
    NACK = 0,

    //! Different edge of the element model has to be chosen.
    DifferentEdge = 0,

    //! XML declaration.
    XmlDecl = 2,

    //! Document type declaration.
    Doctype = 3,

    //! Default data.
    Default = 5,

    //! A declaration of the entity.
    EntityDecl = 6,

    //! A declaration of the notation.
    NotationDecl = 7,

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
    EmptyString = 8
  };
}



/*!
  \brief Structure representing one element context.

  Each element has its own element context which contains some probabilistic information about it.
 */
struct AdaptiveElementContext
{
  //! The name of the element.
  XmlChar *name;

  //! The pointer to the modeler of the element.
  ElementModeler *modeler;
};


//! Hashtable of element contexts (keys are element ids).
typedef HashTable<unsigned long, AdaptiveElementContext, List, 3079> AdaptiveElementTable;

//! Hashtable of attribute names (keys are their ids).
typedef HashTable<unsigned long, XmlChar, List, 3079> AttributeNamesTable;



/*!
  \brief An adaptive XML modeling class.

  This class implements an adaptive model used for encoding and decoding of XML data. When compressing, the model tries to build an oriented graph describing the elements of the document, and to predict the document's structure on the basis of this graphs.
 */
class XmlAdaptiveModel : public XmlModelBase
{
public:
  //! A constructor
  XmlAdaptiveModel(void);
  
  //! A destructor.
  virtual ~XmlAdaptiveModel(void);

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
  //! The pointer to the grammar used for compression.
  GrammarBase *grammar;


  //! Indication of CDATA section.
  bool inCDATA;

  //! Indication of the characters section.
  bool inCharacters;

  //! Indication of the comment section.
  bool inComment;

  /*!
    \brief Indication of the processing instruction section.

    The value 2 indicates that a target is being read, and value 1 indicates that a value is being read.
   */
  char inPI;

  //! indication whether we are in the XML prolog.
  bool inProlog;

  //! Indication whether the model has to perform some NACK operation.
  bool inNACK;

  //! Indication of DOCTYPE section.
  bool inDoctype;

  //! Indication whether the model is processing the attributes of some element.
  bool inAttr;

  //! Indication if the value of some attribute is being read.
  bool inAttrValue;

  //! Indication whether the first attribute of the element has been read.
  bool firstAttrRead;

  //! Indication when NACK caused a step to a character node.
  bool nackEndedInCharacters;

  /*!
    \brief The position when reading the attributes.

    Value 0 indicates that we are starting to read the attributes, and value 1 means that we read all the attributes.
   */
  int attrPos;

  /*!
    \brief State of the model.

    Used during decoding. State is represented by AdaptiveStructuralSymbols.
  */
  AdaptiveStructuralSymbols::Symbol state;

  //! Hashtable of element ids.
  ElementIds *elementIds;

  //! Hashtable of element contexts.
  AdaptiveElementTable *elements;

  //! Hashtable of attribute names.
  AttributeNamesTable *attributeNames;

  //! %List of element names.
  ElementNamesList *elementNamesList;

  //! Counter for unique element numbering.
  unsigned long elementCounter;



  //! Hashtable of attribute ids.
  AttributeIds *attributeIds;

  //! %List of attribute names.
  AttributeNamesList *attributeNamesList;

  //! Counter for unique attribute numbering.
  unsigned long attributeCounter;



  /*!
    \brief Stack of element ids.

    Stores the "level" in the document.
  */
  Stack<unsigned long> *elementStack;


  //! A stack of modelers of the elements.
  Stack<ElementModelerInfo> elementModelerStack;

  //! The list of modelers of the elements.
  List<ElementModeler> elementModelerList;


  //! The value of the clue edge.
  size_t clueEdge;

  //! Indication whether the model is reading the NACK data.
  bool readingNackData;

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

  //! Helper variable for decoding the Fibonacci codes.
  bool fibLastWasOne;

  //! A queue of decoded data items (such as attribute names and values, etc.)
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


#endif //XMLADAPTIVEMODEL_H
