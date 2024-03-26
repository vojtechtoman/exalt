/***************************************************************************
    xmlmodel.cpp  -  Definitions of class XmlSimpleModel methods
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
  \file xmlsimplemodel.cpp
  \brief Definitions of XmlSimpleModel class methods.

  This file contains definitions of XmlSimpleModel class methods.
*/


#ifdef __GNUG__
# pragma implementation
#endif


#include "xmlsimplemodel.h"


/*!
  \brief Call SAX emitter in a safe way.

  The methods of the emitter are called only if the emitter exists.

  \param _x_ The name of the called method of the emitter.
 */
#define SAFE_CALL_EMITTER(_x_)		\
if (saxEmitter)				\
  saxEmitter->_x_;


//! Resets (empties) the input buffer.
#define RESET_BUFFER			\
{					\
  buffer[0] = 0;			\
  bufferLength = 0;			\
}


/*!
  \brief Change the state of the model.

  The state is changed only if current state is \a None.

  \param _s_ The desired state.
 */
#define CHANGE_STATE(_s_)					\
{								\
  stateChanged = false;						\
  if (state != StructuralSymbols::KnownElement &&		\
      (state == StructuralSymbols::None	|| state == _s_))	\
  {								\
    state = _s_;						\
    stateChanged = true;					\
  }								\
}

//! Set the state to \a None.
#define RESET_STATE			state = StructuralSymbols::None


/*!
  Append the \a data of length \a length to the right side of the root rule of the \a grammar. If \a complete is TRUE, insert also "end of chunk symbol".

  \param _gr_ Grammar.
  \param _data_ Appended data.
  \param _length_ Length of the data.
  \param _complete_ Indication if the data are complete.
*/
#define APPEND_ALL_DATA_LENGTH(_gr_, _data_, _length_, _complete_)	\
{									\
  CHECK_POINTER(_gr_);							\
									\
  for (int _i_ = 0; _i_ < _length_; _i_++)				\
    _gr_->append((TerminalValue)_data_[_i_]);				\
									\
  if (_complete_)							\
    _gr_->append(StructuralSymbols::EndOfBlock);			\
}


/*!
  Append the \a data to the right side of the root rule of the \a grammar. If \a complete is TRUE, insert also "end of chunk symbol".

  \param _gr_ Grammar.
  \param _data_ Appended data.
  \param _complete_ Indication if the data are complete.
*/
#define APPEND_ALL_DATA(_gr_, _data_, _complete_)	\
{							\
  CHECK_POINTER(_gr_);					\
							\
  if (!_data_)						\
    return false;					\
							\
  for (int _i_ = 0; _data_[_i_]; _i_++)			\
    _gr_->append((TerminalValue)_data_[_i_]);		\
							\
  if (_complete_)					\
    _gr_->append(StructuralSymbols::EndOfBlock);	\
}


/*!
  Append data from buffer to the right side of the root rule of the grammar.

  \param _gr_ Grammar.
  \param _buffer_ Buffer containing data.
  \param _nrItems_ Number of items in the buffer.
*/
#define APPEND_BUFFER_DATA(_gr_, _buffer_, _nrItems_)	\
{							\
  for (size_t _i_ = 0; _i_ < _nrItems_; _i_++)		\
    {							\
      _gr_->append(_buffer_[_i_]);			\
    }							\
}



/*!
  \brief Finish the element start SAX event.

  The event can be only emitted if all of the attributes and their values have been read.

  \param _elementName_ The name of the element.
 */
#define FINISH_ELEMENT_START(_elementName_)					\
{										\
  if (!elementHasAttributes)							\
    {										\
      /* element hasn't attributes --> we can push it to the stack */		\
      /* and emit startElement with no attributes */				\
      /* push element to the stack */						\
      elementStack->push(new unsigned long(elementId));				\
										\
      SAFE_CALL_EMITTER(startElement(userData, ec->name, 0));			\
										\
      /* Element information is now complete */					\
      elementHasAttributes = false;						\
      attributeListComplete = false;						\
    }										\
  else										\
    if (attributeListComplete)							\
      {										\
        const XmlChar **attrs;							\
        size_t attrPos = 0;							\
										\
        NEW(attrs, const XmlChar *[dataQueue->count()+1]);			\
        attrs[dataQueue->count()] = 0;						\
        /* attributes and the element name decoded */				\
        while ((dataQueueItem = dataQueue->dequeue()))				\
          {									\
            /* an attribute name or an attribute value */			\
            attrs[attrPos] = dataQueueItem->data;				\
            attrPos++;								\
          }									\
										\
        elementStack->push(new unsigned long(elementId));			\
        SAFE_CALL_EMITTER(startElement(userData, _elementName_, attrs));	\
										\
        /* delete attribute values */						\
        /* attribute names can't be deleted, since they	*/			\
        /* are in the element list!			*/			\
        for (attrPos = 0; attrs[attrPos]; attrPos += 2)				\
          {									\
            DELETE(attrs[attrPos+1]);						\
          }									\
										\
        DELETE_ARRAY(attrs);							\
										\
        elementHasAttributes = false;						\
        attributeListComplete = false;						\
      }										\
    else									\
      {										\
        /* some attributes or the element still need to be decoded */		\
        /* enqueue the decoded name				   */		\
        NEW(dataQueueItem, DataQueueItem);					\
        dataQueueItem->type = Characters;					\
        dataQueueItem->data = ec->name;						\
        dataQueue->enqueue(dataQueueItem);					\
      }										\
  RESET_STATE;									\
}



/*!
  Initialization is performed.
 */
XmlSimpleModel::XmlSimpleModel(void)
  : XmlModelBase()
{
  grammar = 0;

  state = StructuralSymbols::None;
  
  buffer = 0;
  bufferLength = 0;

  fibBits = 0;
  fibCode = 0;
  fibLastWasOne = false;
  
  NEW(dataQueue, DataQueue);
  dataQueue->setAutoDelete(true);

  saxEmitter = 0;
  userData = 0;

  //    dataInEncoding = UTF_8;
  //    dataOutEncoding = UTF_8;

  inCDATA = false;
  inDoctype = false;
  elementHasAttributes = false;
  attributeListComplete = false;

  //stack of element ids
  NEW(elementStack, Stack<unsigned long>);

  elementStack->setAutoDelete(true);

  NEW(elementIds, ElementIds);
  elementIds->setAutoDelete(true);

  NEW(elements, SimpleElementTable);
  elements->setAutoDelete(true);

  //list for containing all element nnames
  //used just for freeing all the data
  NEW(elementNamesList, ElementNamesList);
  elementNamesList->setAutoDelete(true);

  elementCounter = 1;
}


/*!
  The resources occupied by the model are freed.
 */  
XmlSimpleModel::~XmlSimpleModel(void)
{
  DELETE(elements);
  DELETE(elementStack);


  //display the number of elements and attributes if in verbose mode
  if (ExaltOptions::getOption(ExaltOptions::Verbose) == ExaltOptions::Yes)
    {
      OUTPUTENL("  Number of elements/attributes:\t" << elementNamesList->count());
      OUTPUTEENDLINE;
    }



  DELETE(elementNamesList);

  DELETE(dataQueue);
}


/*!
  Sets the SAX emitter and (optionaly) the user data.

  \param emitter The SAX emitter.
  \param data The user data structure.
*/
void XmlSimpleModel::setSAXEmitter(SAXEmitter *emitter, void *data = 0)
{
  saxEmitter = emitter;
  userData = data;
}


/*!
  This methods implements the encoding procedure. The model accepts the XML events here and writes the (transformed) data to the underlying grammar.

  \param event The XML event.
*/
bool XmlSimpleModel::manageEvent(XmlModelEvent *event)
{
  XmlStartElementEvent *startElementEvent;
  XmlEndElementEvent *endElementEvent;
  XmlCharactersEvent *charactersEvent;
  XmlCommentEvent *commentEvent;
  XmlPIEvent *piEvent;
  XmlDeclEvent *xmlDeclEvent;
  XmlStartDoctypeEvent *xmlStartDoctypeEvent;

  XmlEntityDeclEvent *xmlEntityDeclEvent;
  XmlNotationDeclEvent *xmlNotationDeclEvent;
  XmlDefaultEvent *defaultEvent;

  XmlChar *elName, *attrName;
  unsigned long *elId, *attrId;
  unsigned long elementId, attributeId;

  SimpleElementContext *ec;
  size_t fibItems;
  XmlChar fibBuf[30];
  Encodings::MIB mib;

  //examine the event type and perform appropriate actions
  switch (event->type)
    {
      /**** XML declaration ****/
    case XmlModelEvent::XmlDecl:
      xmlDeclEvent = (XmlDeclEvent *)event;

      if (!textCodec)
	//use default text codec
	createDefaultTextCodec();

      mib = textCodec->getMIB(xmlDeclEvent->encoding);
	  
      grammar->setAlphabetBaseSize(textCodec->suggestAlphabetBaseSize(mib));

      grammar->append(StructuralSymbols::XmlDecl);

      APPEND_ALL_DATA(grammar, xmlDeclEvent->version, true);

      if (xmlDeclEvent->encoding)
	{
	  APPEND_ALL_DATA(grammar, xmlDeclEvent->encoding, true);
	}
      else
	grammar->append(StructuralSymbols::EndOfBlock);

      if (xmlDeclEvent->standalone != -1)
	{
	  if (xmlDeclEvent->standalone)
	    grammar->append(StructuralSymbols::StandaloneYes);
	  else
	    grammar->append(StructuralSymbols::StandaloneNo);
	}
      else
	grammar->append(StructuralSymbols::StandaloneNotSpecified);

      break;



      /**** Doctype ****/
    case XmlModelEvent::StartDoctype:
      xmlStartDoctypeEvent = (XmlStartDoctypeEvent *)event;

      grammar->append(StructuralSymbols::Doctype);

      if (xmlStartDoctypeEvent->doctypeName)
	{
	  APPEND_ALL_DATA(grammar, xmlStartDoctypeEvent->doctypeName, true);
	}
      else
	grammar->append(StructuralSymbols::EndOfBlock);

      if (xmlStartDoctypeEvent->publicId)
	{
	  if (!xmlStartDoctypeEvent->publicId[0])
	    {
	      grammar->append(StructuralSymbols::EmptyString);
	      grammar->append(StructuralSymbols::EndOfBlock);
	    }
	  else
	    {
	      APPEND_ALL_DATA(grammar, xmlStartDoctypeEvent->publicId, true);
	    }
	}
      else
	grammar->append(StructuralSymbols::EndOfBlock);
	  
      if (xmlStartDoctypeEvent->systemId)
	{
	  if (!xmlStartDoctypeEvent->systemId[0])
	    {
	      grammar->append(StructuralSymbols::EmptyString);
	      grammar->append(StructuralSymbols::EndOfBlock);
	    }
	  else
	    {
	      APPEND_ALL_DATA(grammar, xmlStartDoctypeEvent->systemId, true);
	    }
	}
      else
	grammar->append(StructuralSymbols::EndOfBlock);
	  
      if (xmlStartDoctypeEvent->hasInternalSubset)
	grammar->append(StructuralSymbols::HasInternalSubsetYes);
      else
	grammar->append(StructuralSymbols::HasInternalSubsetNo);
      //  	}
      //        else
      //  	inDoctype = false;

	  
      break;

      /**** End Doctype ****/
    case XmlModelEvent::EndDoctype:
      grammar->append(StructuralSymbols::Doctype);
      break;


      /**** Entity declaration ****/
    case XmlModelEvent::EntityDecl:
      xmlEntityDeclEvent = (XmlEntityDeclEvent *)event;

      grammar->append(StructuralSymbols::EntityDecl);

      if (xmlEntityDeclEvent->entityName)
	{
	  APPEND_ALL_DATA(grammar, xmlEntityDeclEvent->entityName, true);
	}
      else
	grammar->append(StructuralSymbols::EndOfBlock);

     
      if (!xmlEntityDeclEvent->valueLength && xmlEntityDeclEvent->value)
	{
	  //zero length and value not NULL
	  grammar->append(StructuralSymbols::EmptyString);
	  grammar->append(StructuralSymbols::EndOfBlock);
	}
      else
	{
	  if (xmlEntityDeclEvent->value)
	    {
	      //value of nonzero length
	      APPEND_ALL_DATA_LENGTH(grammar, xmlEntityDeclEvent->value, xmlEntityDeclEvent->valueLength, true);
	    }
	  else
	    //value is NULL
	    grammar->append(StructuralSymbols::EndOfBlock);
	}


      if (xmlEntityDeclEvent->systemId)
	{
	  if (!xmlEntityDeclEvent->systemId[0])
	    {
	      grammar->append(StructuralSymbols::EmptyString);
	      grammar->append(StructuralSymbols::EndOfBlock);
	    }
  	  else
	    {
	      APPEND_ALL_DATA(grammar, xmlEntityDeclEvent->systemId, true);
	    }
	}
      else
	grammar->append(StructuralSymbols::EndOfBlock);

      if (xmlEntityDeclEvent->publicId)
	{
	  if (!xmlEntityDeclEvent->publicId[0])
	    {
	      grammar->append(StructuralSymbols::EmptyString);
	      grammar->append(StructuralSymbols::EndOfBlock);
	    }
	  else
	    {
	      APPEND_ALL_DATA(grammar, xmlEntityDeclEvent->publicId, true);
	    }
	}
      else
	grammar->append(StructuralSymbols::EndOfBlock);

      if (xmlEntityDeclEvent->notationName)
	{
	  if (!xmlEntityDeclEvent->notationName[0])
	    {
	      grammar->append(StructuralSymbols::EmptyString);
	      grammar->append(StructuralSymbols::EndOfBlock);
	    }
	  else
	    {
	      APPEND_ALL_DATA(grammar, xmlEntityDeclEvent->notationName, true);
	    }
	}
      else
	grammar->append(StructuralSymbols::EndOfBlock);

      if (xmlEntityDeclEvent->isParameterEntity)
	grammar->append(StructuralSymbols::IsParameterEntityYes);
      else
	grammar->append(StructuralSymbols::IsParameterEntityNo);

      break;


      /**** Notation declaration ****/
    case XmlModelEvent::NotationDecl:
      xmlNotationDeclEvent = (XmlNotationDeclEvent *)event;

      grammar->append(StructuralSymbols::NotationDecl);

      if (xmlNotationDeclEvent->notationName)
	{
	  APPEND_ALL_DATA(grammar, xmlNotationDeclEvent->notationName, true);
	}
      else
	grammar->append(StructuralSymbols::EndOfBlock);

     

      if (xmlNotationDeclEvent->systemId)
	{
	  if (!xmlNotationDeclEvent->systemId[0])
	    {
	      grammar->append(StructuralSymbols::EmptyString);
	      grammar->append(StructuralSymbols::EndOfBlock);
	    }
  	  else
	    {
	      APPEND_ALL_DATA(grammar, xmlNotationDeclEvent->systemId, true);
	    }
	}
      else
	grammar->append(StructuralSymbols::EndOfBlock);

      if (xmlNotationDeclEvent->publicId)
	{
	  if (!xmlNotationDeclEvent->publicId[0])
	    {
	      grammar->append(StructuralSymbols::EmptyString);
	      grammar->append(StructuralSymbols::EndOfBlock);
	    }
	  else
	    {
	      APPEND_ALL_DATA(grammar, xmlNotationDeclEvent->publicId, true);
	    }
	}
      else
	grammar->append(StructuralSymbols::EndOfBlock);


      break;


      /**** Start of element ****/
    case XmlModelEvent::StartElement:
      startElementEvent = (XmlStartElementEvent *)event;

      if (startElementEvent->attr[0])
	{
	  //append "end of attributes" symbol to the grammar
	  //because the element has some attributes
	  grammar->append(StructuralSymbols::Attributes);

	  //encode the attributes
	  for (size_t i = 0; startElementEvent->attr[i]; i += 2)
	    {
	      if (!(attrId = elementIds->find((XmlChar *)startElementEvent->attr[i])))
		{
		  //unknown attribute started
		  //--> we'll create new element context for it



		  NEW(attrName, XmlChar[xmlchar_strlen(startElementEvent->attr[i]) + 1]);
		  xmlchar_strcpy(attrName, startElementEvent->attr[i]);
		  elementNamesList->append(attrName);
      

		  //insert new element id to elementIds
		  attributeId = elementCounter;
		  elementIds->insert(attrName, new unsigned long(attributeId));



		  NEW(ec, SimpleElementContext);

		  //set unique element id
		  ec->name = attrName;


		  //add new element context to the element hashtable
		  elements->insert(attributeId, ec);


		  //update the element counter
		  elementCounter++;



		  //append "new element" symbol to the grammar
		  grammar->append(StructuralSymbols::NewElement);

		  //append the name of the element to the grammar
		  //("end of block" symbol included automatically because of the ending 0
		  APPEND_ALL_DATA(grammar, attrName, true);
		}
	      else
		{
		  //known attribute started
		  //--> update its element context
		  ec = elements->find(*attrId);
		  CHECK_POINTER(ec);

		  attributeId = *attrId;


		  //append "known element" symbol to the grammar
		  grammar->append(StructuralSymbols::KnownElement);
		  fibItems = Fibonacci::encodeToBuffer(fibBuf, SIZEOF_XML_CHAR, attributeId);
		  APPEND_BUFFER_DATA(grammar, fibBuf, fibItems);
		}

	      //append the attribute value
	      if (startElementEvent->attr[i+1][0] == 0)
		grammar->append(StructuralSymbols::EmptyString);
	      else
		APPEND_ALL_DATA(grammar, startElementEvent->attr[i+1], true);
	    }

	  //append "end of block" after the element name to indicate that the description of an element with attributes ended
	  grammar->append(StructuralSymbols::EndOfBlock);

	}

      if (!(elId = elementIds->find((XmlChar *)startElementEvent->name)))
	{
	  //unknown element started
	  //--> we'll create new element context for it



	  NEW(elName, XmlChar[xmlchar_strlen(startElementEvent->name) + 1]);
	  xmlchar_strcpy(elName, startElementEvent->name);
	  elementNamesList->append(elName);
      

	  //insert new element id to elementIds
	  elementId = elementCounter;
	  elementIds->insert(elName, new unsigned long(elementId));

	  //DBG("New element " << (const char *)startElementEvent->name << " (" << elementId << ") started");


	  NEW(ec, SimpleElementContext);

	  //set unique element id
	  ec->name = elName;



	  //add new element context to the element hashtable
	  elements->insert(elementId, ec);


	  //update the element counter
	  elementCounter++;



	  //append "new element" symbol to the grammar
  	  grammar->append(StructuralSymbols::NewElement);

	  //append the name of the element to the grammar
	  //("end of block" symbol included automatically because of the ending 0
	  APPEND_ALL_DATA(grammar, startElementEvent->name, true);

	  //    	  DBG("NEW ELEMENT: "<< startElementEvent->name);
	}
      else
	{
	  //known element started
	  //--> update its element context
	  ec = elements->find(*elId);
	  CHECK_POINTER(ec);

	  elementId = *elId;


	  //  	  DBG("KNOWN: ");

	  //append "known element" symbol to the grammar
  	  grammar->append(StructuralSymbols::KnownElement);

	  fibItems = Fibonacci::encodeToBuffer(fibBuf, SIZEOF_XML_CHAR, elementId);
	  APPEND_BUFFER_DATA(grammar, fibBuf, fibItems);

	  //    	  DBG(startElementEvent->name << " (" << elementId << ") ITEMS: " << fibItems << " CODE: ");
	  //    	  for (unsigned int uu = 0; uu < fibItems; uu++)
	  //    	    {
	  //    	      DBG((unsigned int)(unsigned char)fibBuf[uu] << " ");
	  //    	    }
	}

      //push element name to the stack
      elementStack->push(new unsigned long(elementId));

      break;



      /**** End of element ****/
    case XmlModelEvent::EndElement:
      endElementEvent = (XmlEndElementEvent *)event;

      //SimpleElementContext *ec;

      if (!(elId = elementIds->find((XmlChar *)endElementEvent->name)))
	{
	  //end of unknown element occurred
	  //--> should never happen
	  FATAL("End of unknown element acurred: " << (const char *)endElementEvent->name);
	}
      else
	{
	  //known element ended

	  //	  DBG("  Known element " << (const char *)endElementEvent->name << " ended");
	}

      elementStack->pop();

      //append "end element" symbol to the grammar
      grammar->append(StructuralSymbols::EndElement);

      //APPEND_ALL_DATA(grammar, endElementEvent->name);
      break;



      /**** Character data ****/
    case XmlModelEvent::Characters:
      charactersEvent = (XmlCharactersEvent *)event;

      //        for (int t = 0; t < charactersEvent->length; t++)
      //  	{
      //  	  DBG(charactersEvent->data[t]);
      //  	}
      APPEND_ALL_DATA_LENGTH(grammar, charactersEvent->data, charactersEvent->length, true);
      break;

      /**** Default data ****/
    case XmlModelEvent::Default:
      defaultEvent = (XmlDefaultEvent *)event;

      //append "default" symbol to the grammar
      grammar->append(StructuralSymbols::Default);
      APPEND_ALL_DATA_LENGTH(grammar, defaultEvent->data, defaultEvent->length, true);
      break;


      /**** Comment ****/
    case XmlModelEvent::Comment:
      commentEvent = (XmlCommentEvent *)event;

      //append "comment" symbol to the grammar
      grammar->append(StructuralSymbols::Comment);

      APPEND_ALL_DATA(grammar, commentEvent->data, true);
      break;


      /**** StartCDATA ****/
    case XmlModelEvent::StartCDATA:
      //startCDATAEvent = (XmlStartCDATAEvent *)event;

      //append "comment" symbol to the grammar
      grammar->append(StructuralSymbols::CDATA);
      break;

      /**** EndCDATA ****/
    case XmlModelEvent::EndCDATA:
      //endCDATAEvent = (XmlEndCDATAEvent *)event;

      //append "comment" symbol to the grammar
      grammar->append(StructuralSymbols::CDATA);
      break;



      /**** Processing instruction ****/
    case XmlModelEvent::PI:
      piEvent = (XmlPIEvent *)event;

      //append "pi" symbol to the grammar
      grammar->append(StructuralSymbols::PI);

      if (piEvent->target)
	{
	  //  	  DBG(piEvent->target);
	  APPEND_ALL_DATA(grammar, piEvent->target, true);

	  if (piEvent->data)
	    {
	      //  	      DBG(piEvent->data);
	      APPEND_ALL_DATA(grammar, piEvent->data, true);
	    }
	  else
	    grammar->append(StructuralSymbols::EndOfBlock);
	}
      else
	grammar->append(StructuralSymbols::EndOfBlock);
      
      break;

    default:
      WRN("Unknown XML event: " << event->type);
      DELETE(event);
      return false;
    }

  //delete the event structure
  DELETE(event);
  return true;
}



/*!
  This method implements the decoding procedure. It receives data from the underlying grammar and reconstructs the XML data.

  \param data The data coming from the grammar.
  \param size The length of the data.
 */
void XmlSimpleModel::receiveData(XmlChar *data, size_t size)
{
  bool stateChanged = false;
  DataQueueItem *dataQueueItem;
  SimpleElementContext *ec;
  XmlChar *elName;
  unsigned long elementId;
  unsigned long *elId;

  //some variables for working with Fibonacci codes
  unsigned char fibTmpChar, fibMask;
  bool fibFinished = false;


  if (!buffer)
    {
      NEW(buffer, XmlChar[XML_MODEL_BUFFER_DEFAULT_SIZE]);
      RESET_BUFFER;
    }

  for (size_t i = 0; i < size; i++)
    {
      //        DBG((unsigned int)data[i]);
      if (state != StructuralSymbols::KnownElement)
	{
	  switch (data[i])
	    {
	      //case StructuralSymbols::EndOfBlock:
	      //DBG("END OF BLOCK");
	      //	  state = StructuralSymbols::XmlDecl;
	      //break;

	    case StructuralSymbols::XmlDecl:
	      //DBG("XML DECL");

	      CHANGE_STATE(StructuralSymbols::XmlDecl);
	      if (stateChanged)
		{
		  continue;
		}
	      break;

	    case StructuralSymbols::Attributes:
	      //DBG("ATTRIBUTES");
	      elementHasAttributes = true;

	      //  	  if (!inAttributeSection)
	      //  	    {
	      //  	      //attribute section ended
	      //  	      RESET_BUFFER;
	      //  	      RESET_STATE;

	      //  	      //emit startElementEvent
	      //  	      //	      SAFE_CALL_EMITTER();
	      //  	    }
	      continue;
	      break;

	    case StructuralSymbols::NewElement:
	      //DBG("NEW ELEMENT");

	      CHANGE_STATE(StructuralSymbols::NewElement);
	      if (stateChanged)
		{
		  continue;
		}
	      break;

	    case StructuralSymbols::KnownElement:
	      //DBG("KNOWN ELEMENT");

	      CHANGE_STATE(StructuralSymbols::KnownElement);
	      if (stateChanged)
		{
		  continue;
		}
	      break;

	    case StructuralSymbols::EndElement:
	      //DBG("END ELEMENT: ");

	      CHANGE_STATE(StructuralSymbols::None);

	      if (stateChanged)
		{
		  elId = elementStack->pop();
		  CHECK_POINTER(elId);
		  ec = elements->find(*elId);

		  //DBG("** " << ec->name);
	      
		  SAFE_CALL_EMITTER(endElement(userData, ec->name));
		  continue;
		}
	      else
		{
		  //DBG("!!!!!!!!! " << state);
		  //DBG((unsigned int)data[i]);
		}

	      break;

	    case StructuralSymbols::Default:
	      //DBG("COMMENT");
	      CHANGE_STATE(StructuralSymbols::Default);
	      if (stateChanged)
		{
		  continue;
		}
	      break;


	    case StructuralSymbols::Comment:
	      //DBG("COMMENT");
	      CHANGE_STATE(StructuralSymbols::Comment);
	      if (stateChanged)
		{
		  continue;
		}
	      break;

	    case StructuralSymbols::CDATA:
	      //DBG("CDATA");
	      if (!inCDATA)
		{
		  CHANGE_STATE(StructuralSymbols::CDATA);
		  if (stateChanged)
		    {
		      SAFE_CALL_EMITTER(startCDATASection(userData));
		      inCDATA = true;
		      continue;
		    }
		}
	      else
		{
		  CHANGE_STATE(StructuralSymbols::None);
		  if (stateChanged)
		    {
		      SAFE_CALL_EMITTER(endCDATASection(userData));
		      inCDATA = false;
		      continue;
		    }
		}
	      break;

	    case StructuralSymbols::PI:
	      //  	  DBG("PI");
	      CHANGE_STATE(StructuralSymbols::PI);
	      if (stateChanged)
		{
		  continue;
		}
	      break;

	    case StructuralSymbols::EntityDecl:
	      //  	  DBG("ENTITY DECL");
	      CHANGE_STATE(StructuralSymbols::EntityDecl);
	      if (stateChanged)
		{
		  continue;
		}
	      break;

	    case StructuralSymbols::NotationDecl:
	      //  	  DBG("NOTATION DECL");
	      CHANGE_STATE(StructuralSymbols::NotationDecl);
	      if (stateChanged)
		{
		  continue;
		}
	      break;

	    case StructuralSymbols::Doctype:
	      //  	  DBG("DOCTYPE");
	      if (!inDoctype)
		{
		  CHANGE_STATE(StructuralSymbols::Doctype);
		  inDoctype = true;

		  if (stateChanged)
		    {
		      continue;
		    }
		}
	      else
		{
		  SAFE_CALL_EMITTER(endDoctypeDecl(userData));
		  inDoctype = false;
		  continue;
		}
	      break;

	    default:
	      if (!data[i] && !bufferLength && elementHasAttributes)
		{
		  //end of the attribute section of the element
		  attributeListComplete = true;
		  continue;
		}
	    }
	}

      switch (state)
	{
	  //  	    case StructuralSymbols::EndOfBlock:
	  //  	      //state = StructuralSymbols::None;
	  //  	      break;
	  
	case StructuralSymbols::XmlDecl:
	  //DBG("comparing: " << (int)data[i]);
	  if (data[i] == StructuralSymbols::StandaloneYes || data[i] == StructuralSymbols::StandaloneNo
	      || (data[i] == StructuralSymbols::StandaloneNotSpecified && dataQueue->count() == 2))
	    {
	      //DBG("!!!!");
	      //read standalone information
	      XmlChar *version;
	      XmlChar *encoding;
	      int standalone;

	      switch(data[i])
		{
		case StructuralSymbols::StandaloneYes:
		  standalone = 1;
		  break;

		case StructuralSymbols::StandaloneNo:
		  standalone = 0;
		  break;

		default:
		  standalone = -1;
		}

	      version = dataQueue->dequeue()->data;
	      encoding = dataQueue->dequeue()->data;

	      SAFE_CALL_EMITTER(xmlDecl(userData, version, encoding, standalone));

	      if (version)
		DELETE(version);

	      if (encoding)
		DELETE(encoding);

	      RESET_STATE;
	      RESET_BUFFER;
	    }
	  else
	    if (data[i] == StructuralSymbols::EndOfBlock)
	      {
		//DBG("@");
		//read version or encoding
		NEW(dataQueueItem, DataQueueItem);
		dataQueueItem->type = Characters;

		if (bufferLength)
		  {
		    buffer[bufferLength] = 0;
		    NEW(dataQueueItem->data, XmlChar[xmlchar_strlen(buffer)+1]);
		    xmlchar_strcpy(dataQueueItem->data, buffer);
		  }
		else
		  //data not present (can occur for encoding)
		  dataQueueItem->data = 0;

		dataQueue->enqueue(dataQueueItem);

		RESET_BUFFER;

	      }
	    else
	      {
		//read next character of xml declaration data
		buffer[bufferLength] = data[i];
		bufferLength++;
	      }
	  break;

	case StructuralSymbols::PI:
	  if (data[i] == StructuralSymbols::EndOfBlock && dataQueue->count() == 1)
	    {
	      XmlChar *target;

	      target = dataQueue->dequeue()->data;

	      buffer[bufferLength] = 0;
	      //value was specified
	      SAFE_CALL_EMITTER(processingInstruction(userData, target, buffer));

	      if (target)
		DELETE(target);

	      RESET_BUFFER;
	      RESET_STATE;
	    }
	  else
	    if (data[i] == StructuralSymbols::EndOfBlock)
	      {
		//read target value
		NEW(dataQueueItem, DataQueueItem);
		dataQueueItem->type = Characters;

		if (bufferLength)
		  {
		    buffer[bufferLength] = 0;
		    NEW(dataQueueItem->data, XmlChar[xmlchar_strlen(buffer)+1]);
		    xmlchar_strcpy(dataQueueItem->data, buffer);
		  }
		else
		  //data not present (can occur for encoding)
		  dataQueueItem->data = 0;

		dataQueue->enqueue(dataQueueItem);

		RESET_BUFFER;

	      }
	    else
	      {
		//read next character of xml target or value data
		buffer[bufferLength] = data[i];
		bufferLength++;
	      }
	  break;

	case StructuralSymbols::NewElement:
	  if (data[i] == StructuralSymbols::EndOfBlock)
	    {
	      //decoding new element


	      //add ending \0
	      buffer[bufferLength] = 0;
	      bufferLength++;

	      //save element name
	      NEW(elName, XmlChar[bufferLength]);
	      xmlchar_strcpy(elName, buffer);
	      elementNamesList->append(elName);

	      //create an  element contex for the new element
	      NEW(ec, SimpleElementContext);
		  
	      //set unique element id
	      ec->name = elName;
		  
	      elementId = elementCounter;
		  
	      //add new element context to the element hashtable

	      elements->insert(elementCounter, ec);
		  
	      //update the element counter
	      elementCounter++;
		  

	      FINISH_ELEMENT_START(elName);

	      //reset buffer
	      RESET_BUFFER;
	    }
	  else
	    {
	      //read next character of the incomplete element name
	      buffer[bufferLength] = data[i];
	      bufferLength++;
	    }
	  break;

	case StructuralSymbols::KnownElement:
	  //known element started
	  //--> read complete Fibonacci code of its id

	  //construct reverse Fibonacci code (e.g. 1100101 instead of 1010011) from sequence of
	  //following bytes
	  fibMask = 1 << (SIZEOF_CHAR*8 - 1);
	      
	  //decode symbol using Fibonacci code
	  fibTmpChar = (unsigned char)data[i];
  	  //DBG("SSS: " << (unsigned int)fibTmpChar);

	  while (fibMask)
	    {
	      fibCode >>= 1;
	      fibBits++;
		  
	      if (fibTmpChar & fibMask)
		{
		  fibCode = fibCode | ((unsigned long)1) << (SIZEOF_UNSIGNED_LONG*8-1);
		      
		  if (!fibLastWasOne)
		    fibLastWasOne = true;
		  else
		    {
		      //Fibonacci code is now complete
		      //--> decode the symbol number
		      fibCode >>= (SIZEOF_UNSIGNED_LONG*8 - fibBits);
		      elementId = Fibonacci::decode(fibCode);
		      fibFinished = true;
		      break;
		    }
		}
	      else
		fibLastWasOne = false;
		  
	      fibMask >>= 1;
	    }

	  if (fibFinished)
	    {
  	      //DBG("FIB DECODED: " << elementId);
	      ec = elements->find(elementId);

	      CHECK_POINTER(ec);


	      FINISH_ELEMENT_START(ec->name);


	      //complete Fibonacci code read and decoded
	      //reset fibCode, and fibBits
	      fibCode = 0;
	      fibBits = 0;
	      fibLastWasOne = false;

	      RESET_BUFFER;
	      RESET_STATE;
	    }

	  break;

	case StructuralSymbols::Default:
	  //comment data
	  if (data[i] == StructuralSymbols::EndOfBlock)
	    {
	      //buffer[bufferLength] = data[i];
	      SAFE_CALL_EMITTER(defaultHandler(userData, buffer, bufferLength));
	      RESET_STATE;
	      RESET_BUFFER;
	    }
	  else
	    {
	      //read next character of the default data
	      buffer[bufferLength] = data[i];
	      bufferLength++;

	      if (bufferLength == XML_MODEL_BUFFER_DEFAULT_SIZE)
		{
		  //data larger than the buffer --> split it
		  SAFE_CALL_EMITTER(defaultHandler(userData, buffer, bufferLength));
		  RESET_BUFFER;
		}
	    }

	  break;

	case StructuralSymbols::Comment:
	  //comment data
	  if (data[i] == StructuralSymbols::EndOfBlock)
	    {
	      buffer[bufferLength] = 0;
	      SAFE_CALL_EMITTER(comment(userData, buffer));
	      RESET_STATE;
	      RESET_BUFFER;
	    }
	  else
	    {
	      //read next character of the comment data
	      buffer[bufferLength] = data[i];
	      bufferLength++;

	      if (bufferLength == XML_MODEL_BUFFER_DEFAULT_SIZE)
		{
		  //data larger than the buffer --> split it
		  SAFE_CALL_EMITTER(comment(userData, buffer));
		  RESET_BUFFER;
		}
	    }

	  break;

	  //  	    case StructuralSymbols::PI:
	  //  	      RESET_STATE;
	  //  	      break;

	case StructuralSymbols::Doctype:
	  if (/*data[i] == StructuralSymbols::EndOfBlock &&*/ dataQueue->count() == 3)
	    {
	      XmlChar *doctype, *publicId, *systemId;

	      doctype = dataQueue->dequeue()->data;
	      publicId = dataQueue->dequeue()->data;
	      systemId = dataQueue->dequeue()->data;

	      SAFE_CALL_EMITTER(startDoctypeDecl(userData, doctype, systemId, publicId,
						 data[0] == StructuralSymbols::HasInternalSubsetYes));

	      if (doctype)
		DELETE(doctype);

	      if (publicId)
		DELETE(publicId);

	      if (systemId)
		DELETE(systemId);

	      RESET_BUFFER;
	      RESET_STATE;
	    }
	  else
	    if (data[i] == StructuralSymbols::EndOfBlock)
	      {
		//read target value
		NEW(dataQueueItem, DataQueueItem);
		dataQueueItem->type = Characters;

		if (bufferLength)
		  {
		    if (buffer[0] == StructuralSymbols::EmptyString)
		      //we read an empty string
		      buffer[0] = 0;
		    else
		      buffer[bufferLength] = 0;

		    NEW(dataQueueItem->data, XmlChar[xmlchar_strlen(buffer)+1]);
		    xmlchar_strcpy(dataQueueItem->data, buffer);
		  }
		else
		  //data not present
		  dataQueueItem->data = 0;

		dataQueue->enqueue(dataQueueItem);

		RESET_BUFFER;
	      }
	    else
	      {
		//read next character
		buffer[bufferLength] = data[i];
		bufferLength++;
	      }
	  break;


	case StructuralSymbols::EntityDecl:
	  if (dataQueue->count() == 5)	//minus base!
	    {
	      //readind isparameterentity (the last record)
	      XmlChar *entityName, *value, /* *base,*/ *publicId, *systemId, *notationName;
	      int valueLength;

	      entityName = dataQueue->dequeue()->data;
	      value = dataQueue->dequeue()->data;
	      //  	      base = dataQueue->dequeue()->data;
	      systemId = dataQueue->dequeue()->data;
	      publicId = dataQueue->dequeue()->data;
	      notationName = dataQueue->dequeue()->data;


	      if (!value)
		valueLength = 0;
	      else
		valueLength = xmlchar_strlen(value);

	      SAFE_CALL_EMITTER(entityDecl(userData, entityName, data[0] == StructuralSymbols::IsParameterEntityYes,
					   value, valueLength, 0/*base*/, systemId, publicId, notationName));

	      if (entityName)
		DELETE(entityName);

	      if (value)
		DELETE(value);

	      //  	      if (base)
	      //  		DELETE(base);

	      if (systemId)
		DELETE(systemId);

	      if (publicId)
		DELETE(publicId);

	      if (notationName)
		DELETE(notationName);

	      RESET_BUFFER;
	      RESET_STATE;
	    }
	  else
	    if (data[i] == StructuralSymbols::EndOfBlock)
	      {
		//read target value
		NEW(dataQueueItem, DataQueueItem);
		dataQueueItem->type = Characters;

		if (bufferLength)
		  {
		    if (buffer[0] == StructuralSymbols::EmptyString)
		      //we read an empty string
		      buffer[0] = 0;
		    else
		      buffer[bufferLength] = 0;

		    NEW(dataQueueItem->data, XmlChar[xmlchar_strlen(buffer)+1]);
		    xmlchar_strcpy(dataQueueItem->data, buffer);
		  }
		else
		  //data not present
		  dataQueueItem->data = 0;

		dataQueue->enqueue(dataQueueItem);

		RESET_BUFFER;
	      }
	    else
	      {
		//read next character
		buffer[bufferLength] = data[i];
		bufferLength++;
	      }

	  break;

	case StructuralSymbols::NotationDecl:
	  if (data[i] == StructuralSymbols::EndOfBlock)
	    {
	      //read target value
	      NEW(dataQueueItem, DataQueueItem);
	      dataQueueItem->type = Characters;
	      
	      if (bufferLength)
		{
		  if (buffer[0] == StructuralSymbols::EmptyString)
		    //we read an empty string
		    buffer[0] = 0;
		  else
		    buffer[bufferLength] = 0;
		  
		  NEW(dataQueueItem->data, XmlChar[xmlchar_strlen(buffer)+1]);
		  xmlchar_strcpy(dataQueueItem->data, buffer);
		}
	      else
		//data not present
		dataQueueItem->data = 0;
	      
	      dataQueue->enqueue(dataQueueItem);
	      
	      RESET_BUFFER;

	      //complete information read?
	      if (dataQueue->count() == 3)	//minus base!
		{
		  //readind isparameterentity (the last record)
		  XmlChar *notationName, /* *base,*/ *publicId, *systemId;
		  
		  notationName = dataQueue->dequeue()->data;
		  //  	      base = dataQueue->dequeue()->data;
		  systemId = dataQueue->dequeue()->data;
		  publicId = dataQueue->dequeue()->data;
		  
		  SAFE_CALL_EMITTER(notationDecl(userData, notationName, 0/*base*/, systemId, publicId));

		  if (notationName)
		    DELETE(notationName);

		  //  	          if (base)
		  //  		    DELETE(base);

		  if (systemId)
		    DELETE(systemId);
		  
		  if (publicId)
		    DELETE(publicId);
		  
		  if (notationName)
		    DELETE(notationName);

		  RESET_STATE;
		}
	    }
	  else
	    {
	      //read next character
	      buffer[bufferLength] = data[i];
	      bufferLength++;
	    }

	  break;

	default:
	  //characters
	  if (data[i] == StructuralSymbols::EmptyString)
	    {
	      //empty string is ignored unless it's an empty value of an attribute
	      if (elementHasAttributes)
		{
		  //read a value of an attribute
		  XmlChar *attrValue;
		  
		  //copy the value from buffer to attrValue
		  NEW(attrValue, XmlChar[1]);
		  attrValue[0] = 0;
		  
		  NEW(dataQueueItem, DataQueueItem);
		  dataQueueItem->type = Characters;
		  dataQueueItem->data = attrValue;
		  dataQueue->enqueue(dataQueueItem);
		}
	    }
	  else
	    if (data[i] == StructuralSymbols::EndOfBlock)
	      {
		if (elementHasAttributes)
		  {
		    //read a value of an attribute
		    XmlChar *attrValue;

		    //copy the value from buffer to attrValue
		    NEW(attrValue, XmlChar[bufferLength + 1]);
		    buffer[bufferLength] = 0;
		    xmlchar_strcpy(attrValue, buffer);

		    NEW(dataQueueItem, DataQueueItem);
		    dataQueueItem->type = Characters;
		    dataQueueItem->data = attrValue;
		    dataQueue->enqueue(dataQueueItem);
		  }
		else
		  {
		    if (bufferLength)
		      //send data only if they aren't empty
		      SAFE_CALL_EMITTER(characterData(userData, buffer, bufferLength));
		  }

		RESET_BUFFER;
		RESET_STATE;
	      }
	    else
	      {
		buffer[bufferLength] = data[i];
		bufferLength++;
		  
		if (bufferLength == XML_MODEL_BUFFER_DEFAULT_SIZE)
		  {
		    //character data larger than the buffer --> split it
		    SAFE_CALL_EMITTER(characterData(userData, buffer, bufferLength));
		    RESET_BUFFER;
		  }
	      }
	}
    }
}
