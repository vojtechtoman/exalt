/***************************************************************************
    xmlmodelbase.h  -  Definition of XmlModelBase class
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
  \file xmlmodelbase.h
  \brief Definition of XmlModelBase class.
  
  This file contains the definition of XmlModelBase class.
*/


#ifndef XMLMODELBASE_H
#define XMLMODELBASE_H

#include "defs.h"
#include "encodings.h"
#include "grammarbase.h"
#include "hashtable.h"
#include "list.h"
#include "queue.h"
#include "textcodec.h"
#include "xmlmodelevents.h"
#include "saxemitter.h"
#include "funneldevice.h"






//! Default size of the data buffer.
#define XML_MODEL_BUFFER_DEFAULT_SIZE		1024




//! An enum describing the possible types of items in the DataQueue.
enum DataQueueItemType
{
  //! A numeric value.
  Number,

  //! A string value.
  Characters,

  //! A string with given length.
  CharactersLength
};


//! One item in the DataQueue.
struct DataQueueItem
{
  //! The type of the item.
  DataQueueItemType type;

  //! The data of the item.
  XmlChar *data;

  union
  {
    //! The length of the string data.
    int length;

    //! The numeric value of the item.
    int value;
  };
};


/*!
  \brief A queue of items of type DataQueueItem.

  Used by XML models to store some temporary data such as the names of the attributes and their values.
 */
typedef Queue<DataQueueItem> DataQueue;


/*!
  \brief %List of theelement names.

  Used only for convenience and for automatic deallocation of the element names from memory.
*/
typedef List<XmlChar> ElementNamesList;


/*!
  \brief %List of the atribute names.

  Used only for convenience and for automatic deallocation of the attribute names from memory.
*/
typedef List<XmlChar> AttributeNamesList;


//! Hashtable of the element ids (keys are the element names).
typedef HashTable<XmlChar *, unsigned long, List, 3079> ElementIds;

//! Hashtable of the attribute ids (keys are the attribute names).
typedef HashTable<XmlChar *, unsigned long, List, 3079> AttributeIds;



/*!
  \brief An abstract predecessor of the XML models.

  Implements an unified interface for the XML models.
 */
class XmlModelBase : public UserOfTextCodec, public UserOfFunnelDevice
{
public:
  //! A constructor.
  XmlModelBase(void) : UserOfTextCodec(), UserOfFunnelDevice() {}

  /*!
    \brief A destructor.

    Does nothing.
   */
  virtual ~XmlModelBase(void) {}

  /*!
    \brief Manage one XML event.

    This method should implement the encoding functionality of the model.

    \param event A XML event.
   */
  virtual bool manageEvent(XmlModelEvent *event) = 0;

  /*!
    \brief Receive the data from another object.

    This method should implement the decoding functionality of the model.

    \param data The incoming data.
    \param size The size of the data.
   */
  virtual void receiveData(XmlChar *data, size_t size) = 0;

  //! Set the grammar for compression.
  virtual void setGrammar(GrammarBase *gr) = 0;

  //! Set the SAX emitter.
  virtual void setSAXEmitter(SAXEmitter *emitter, void *data = 0) = 0;
};


#endif //XMLMODELBASE_H
