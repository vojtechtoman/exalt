/***************************************************************************
    xmladaptivemodel.cpp  -  Definitions of class XmlAdaptiveModel methods
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
  \file xmladaptivemodel.cpp
  \brief Definitions of XmlAdaptiveModel class methods.

  This file contains definitions of XmlAdaptiveModel class methods.
*/


#ifdef __GNUG__
# pragma implementation
#endif


#include "xmladaptivemodel.h"


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
#define CHANGE_STATE(_s_)						\
{									\
  stateChanged = false;							\
  if (state != AdaptiveStructuralSymbols::KnownElement &&		\
      (state == AdaptiveStructuralSymbols::None	|| state == _s_))	\
  {									\
    state = _s_;							\
    stateChanged = true;						\
  }									\
}


//! Set the state to \a None.
#define RESET_STATE		state = AdaptiveStructuralSymbols::None


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
    _gr_->append(AdaptiveStructuralSymbols::EndOfBlock);		\
}


/*!
  Append the \a data of length \a length to the right side of the root rule of the \a grammar. If \a complete is TRUE, insert also "end of chunk symbol".

  \param _gr_ Grammar.
  \param _data_ Appended data.
  \param _length_ Length of the data.
  \param _complete_ Indication if the data are complete.
*/
#define APPEND_ALL_DATA_LENGTH_CONVERT_AMP(_gr_, _data_, _length_, _complete_)	\
{										\
  CHECK_POINTER(_gr_);								\
										\
  for (int _i_ = 0; _i_ < _length_; _i_++)					\
    {										\
      if (_data_[_i_] == '&')							\
	_gr_->append((TerminalValue)AMP_REPLACEMENT);				\
      else									\
	_gr_->append((TerminalValue)_data_[_i_]);				\
    }  										\
										\
  if (_complete_)								\
    _gr_->append(AdaptiveStructuralSymbols::EndOfBlock);			\
}


/*!
  Append the \a data to the right side of the root rule of the \a grammar. If \a complete is TRUE, insert also "end of chunk symbol".

  \param _gr_ Grammar.
  \param _data_ Appended data.
  \param _complete_ Indication if the data are complete.
*/
#define APPEND_ALL_DATA(_gr_, _data_, _complete_)		\
{								\
  CHECK_POINTER(_gr_);						\
								\
  if (!_data_)							\
    return false;						\
								\
  for (int _i_ = 0; _data_[_i_]; _i_++)				\
    _gr_->append((TerminalValue)_data_[_i_]);			\
								\
  if (_complete_)						\
    _gr_->append(AdaptiveStructuralSymbols::EndOfBlock);	\
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
  \brief End the character data section.

  Appends ending zero to the grammar.

  \param _gr_ The pointer to the grammar.
*/
#define END_CHARACTER_DATA_SECTION(_gr_)		\
{							\
  _gr_->append(AdaptiveStructuralSymbols::EndOfBlock);	\
  inCharacters = false;					\
}


//! Resets the clueEdge value.
#define RESET_CLUE_EDGE				\
{						\
  clueEdge = 1;					\
  DBG("CLUE EDGE RESET!");\
}


//! Increases the value of clueEdge
#define INCREASE_CLUE_EDGE			\
{						\
  clueEdge++;					\
  DBG("value of clue edge after the incrementation " << clueEdge);\
}


//! Appends the NACK informations to the grammar.
#define REPORT_NACK								\
{										\
  DBG("NACK edge: " << clueEdge);						\
										\
  grammar->append(AdaptiveStructuralSymbols::NACK);				\
  fibItems = Fibonacci::encodeToBuffer(fibBuf, SIZEOF_XML_CHAR, clueEdge);	\
  APPEND_BUFFER_DATA(grammar, fibBuf, fibItems);				\
  DBG("FIB ITEMS: " << (unsigned int)fibBuf[0]);				\
}
							

/*!
  \brief Appends data reporting a different edge to the grammar.			

  \param _edge_ Id of selected edge.
*/
#define REPORT_DIFFERENT_EDGE(_edge_)						\
{										\
  DBG("edge no. " << edge << " selected");					\
  grammar->append(AdaptiveStructuralSymbols::DifferentEdge);			\
  fibItems = Fibonacci::encodeToBuffer(fibBuf, SIZEOF_XML_CHAR, _edge_);	\
  APPEND_BUFFER_DATA(grammar, fibBuf, fibItems);				\
}


//! If the current element has no attributes, this macro emits appropriate SAX event.
#define FINISH_START_ELEMENT_WITH_NO_ATTRIBUTES					\
{										\
  if (!attrPos && !dataQueue->isEmpty())					\
    {										\
      DBG("FINISHING WITH NO ATTRS!");						\
      dataQueueItem = dataQueue->dequeue();					\
      /* update the attribute stats */						\
      elementModelerStack.top()->modeler->setAttributes(false);			\
      /* emit startElement event with no attributes */				\
      SAFE_CALL_EMITTER(startElement(userData, dataQueueItem->data, 0));	\
      attrPos = 1;								\
    }										\
}



/*!
  Initialization is performed.
*/
XmlAdaptiveModel::XmlAdaptiveModel(void)
  : XmlModelBase()
{
  grammar = 0;


  state = AdaptiveStructuralSymbols::None;
  
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
  inCharacters = false;
  inProlog = true;
  inNACK = false;
  inAttr= false;
  firstAttrRead = false;
  inAttrValue = false;
  nackEndedInCharacters = false;

  attrPos = 1;	//0 = in the attributes, 1 = not in the attributes


  //stack of element ids
  NEW(elementStack, Stack<unsigned long>);

  elementStack->setAutoDelete(true);

  NEW(elementIds, ElementIds);
  elementIds->setAutoDelete(true);

  NEW(elements, AdaptiveElementTable);
  elements->setAutoDelete(true);


  //list containing all element names
  //used just for freeing all the data
  NEW(elementNamesList, ElementNamesList);
  elementNamesList->setAutoDelete(true);


  NEW(attributeIds, AttributeIds);
  attributeIds->setAutoDelete(true);

  //list containing all attributes names
  //used just for freeing all the data
  NEW(attributeNamesList, AttributeNamesList);
  attributeNamesList->setAutoDelete(true);

  NEW(attributeNames, AttributeNamesTable);
  attributeNames->setAutoDelete(false);		//the attribute names will be deleted by attributeNamesList!

  elementCounter = 1;
  attributeCounter = 1;

  elementModelerList.setAutoDelete(true);

  
  clueEdge = 1;
  readingNackData = false;
  elementModelerStack.setAutoDelete(true);
}


  
/*!
  The resources occupied by the model are freed. In verbose mode, the information about the entropy of the document is displayed.
*/  
XmlAdaptiveModel::~XmlAdaptiveModel(void)
{
  DELETE(elements);
  DELETE(elementStack);


  DELETE(dataQueue);

  DELETE(attributeIds);
  DELETE(attributeNames);

  //pre-compute the structural entropy of the element models if in verbose (or "print models") mode
  if (ExaltOptions::getOption(ExaltOptions::PrintModels) == ExaltOptions::Yes ||
      ExaltOptions::getOption(ExaltOptions::Verbose) == ExaltOptions::Yes)
    {
      for (ElementModeler *em = elementModelerList.first(); em; em = elementModelerList.next())
	{
	  //compute the structural entropy of the models
	  //also, as a side effect, the ref counts of the models are adjusted
	  em->computeStructuralEntropy();
	}
    }
      
  //display the document entropy information if in verbose mode
  if (ExaltOptions::getOption(ExaltOptions::Verbose) == ExaltOptions::Yes)
    {
      size_t totalModels = 0;
      double totalStructuralEntropy = 0;
      for (ElementModeler *em = elementModelerList.first(); em; em = elementModelerList.next())
	{
	  totalModels += em->getRefCount();
	  //DBG("Model for element " << em->getElementName() << " referenced " << em->getRefCount() << " times and has entropy: " << em->getStructuralEntropy());
	  totalStructuralEntropy += em->getRefCount()*em->getStructuralEntropy();
	}
      
      totalStructuralEntropy = totalStructuralEntropy / totalModels;
      
      OUTPUTENL("  Number of elements:\t\t\t" << elementNamesList->count());
      OUTPUTENL("  Number of attributes:\t\t\t" << attributeNamesList->count());
      OUTPUTENL("  Structural entropy of the document:\t" << totalStructuralEntropy);
      OUTPUTEENDLINE;
    }


  //display the element models if in "print models" mode
  if (ExaltOptions::getOption(ExaltOptions::PrintModels) == ExaltOptions::Yes)
    {
      OUTPUTENL("Models of the elements");

      for (ElementModeler *em = elementModelerList.first(); em; em = elementModelerList.next())
	{
	  em->print();
	}
    }


  DELETE(elementNamesList);
  DELETE(attributeNamesList);
}


/*!
  Sets the SAX emitter and (optionaly) the user data.

  \param emitter The SAX emitter.
  \param data The user data structure.
*/
void XmlAdaptiveModel::setSAXEmitter(SAXEmitter *emitter, void *data = 0)
{
  saxEmitter = emitter;
  userData = data;
}


/*!
  This methods implements the encoding procedure. The model accepts the XML events here and writes the (transformed) data to the underlying grammar.

  \param event The XML event.
*/
bool XmlAdaptiveModel::manageEvent(XmlModelEvent *event)
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

  XmlChar *elName;
  unsigned long *elId;
  unsigned long elementId;
  ElementModelerInfo *emi;

  XmlChar *attrName;
  unsigned long *attrId;
  unsigned long attributeId;

  AdaptiveElementContext *ec;
  size_t fibItems;
  XmlChar fibBuf[30];
  Encodings::MIB mib;

  ElementModel::TransitionState result;
  size_t edge;
  size_t elts = 0;
  bool clueEdgeReset = false;

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

      grammar->append(AdaptiveStructuralSymbols::XmlDecl);

      APPEND_ALL_DATA(grammar, xmlDeclEvent->version, true);

      if (xmlDeclEvent->encoding)
	{
	  APPEND_ALL_DATA(grammar, xmlDeclEvent->encoding, true);
	}
      else
	grammar->append(AdaptiveStructuralSymbols::EndOfBlock);

      if (xmlDeclEvent->standalone != -1)
	{
	  if (xmlDeclEvent->standalone)
	    grammar->append(AdaptiveStructuralSymbols::StandaloneYes);
	  else
	    grammar->append(AdaptiveStructuralSymbols::StandaloneNo);
	}
      else
	grammar->append(AdaptiveStructuralSymbols::StandaloneNotSpecified);

      break;



      /**** Doctype ****/
    case XmlModelEvent::StartDoctype:
      xmlStartDoctypeEvent = (XmlStartDoctypeEvent *)event;

      grammar->append(AdaptiveStructuralSymbols::Doctype);

      if (xmlStartDoctypeEvent->doctypeName)
	{
	  APPEND_ALL_DATA(grammar, xmlStartDoctypeEvent->doctypeName, true);
	}
      else
	grammar->append(AdaptiveStructuralSymbols::EndOfBlock);

      if (xmlStartDoctypeEvent->publicId)
	{
	  if (!xmlStartDoctypeEvent->publicId[0])
	    {
	      grammar->append(AdaptiveStructuralSymbols::EmptyString);
	      grammar->append(AdaptiveStructuralSymbols::EndOfBlock);
	    }
	  else
	    {
	      APPEND_ALL_DATA(grammar, xmlStartDoctypeEvent->publicId, true);
	    }
	}
      else
	grammar->append(AdaptiveStructuralSymbols::EndOfBlock);
	  
      if (xmlStartDoctypeEvent->systemId)
	{
	  if (!xmlStartDoctypeEvent->systemId[0])
	    {
	      grammar->append(AdaptiveStructuralSymbols::EmptyString);
	      grammar->append(AdaptiveStructuralSymbols::EndOfBlock);
	    }
	  else
	    {
	      APPEND_ALL_DATA(grammar, xmlStartDoctypeEvent->systemId, true);
	    }
	}
      else
	grammar->append(AdaptiveStructuralSymbols::EndOfBlock);
	  
      if (xmlStartDoctypeEvent->hasInternalSubset)
	grammar->append(AdaptiveStructuralSymbols::HasInternalSubsetYes);
      else
	grammar->append(AdaptiveStructuralSymbols::HasInternalSubsetNo);
	  
      break;

      /**** End Doctype ****/
    case XmlModelEvent::EndDoctype:
      grammar->append(AdaptiveStructuralSymbols::Doctype);
      break;


      /**** Entity declaration ****/
    case XmlModelEvent::EntityDecl:
      xmlEntityDeclEvent = (XmlEntityDeclEvent *)event;

      grammar->append(AdaptiveStructuralSymbols::EntityDecl);

      if (xmlEntityDeclEvent->entityName)
	{
	  APPEND_ALL_DATA(grammar, xmlEntityDeclEvent->entityName, true);
	}
      else
	grammar->append(AdaptiveStructuralSymbols::EndOfBlock);

     
      if (!xmlEntityDeclEvent->valueLength && xmlEntityDeclEvent->value)
	{
	  //zero length and value not NULL
	  grammar->append(AdaptiveStructuralSymbols::EmptyString);
	  grammar->append(AdaptiveStructuralSymbols::EndOfBlock);
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
	    grammar->append(AdaptiveStructuralSymbols::EndOfBlock);
	}

      if (xmlEntityDeclEvent->systemId)
	{
	  if (!xmlEntityDeclEvent->systemId[0])
	    {
	      grammar->append(AdaptiveStructuralSymbols::EmptyString);
	      grammar->append(AdaptiveStructuralSymbols::EndOfBlock);
	    }
  	  else
	    {
	      APPEND_ALL_DATA(grammar, xmlEntityDeclEvent->systemId, true);
	    }
	}
      else
	grammar->append(AdaptiveStructuralSymbols::EndOfBlock);

      if (xmlEntityDeclEvent->publicId)
	{
	  if (!xmlEntityDeclEvent->publicId[0])
	    {
	      grammar->append(AdaptiveStructuralSymbols::EmptyString);
	      grammar->append(AdaptiveStructuralSymbols::EndOfBlock);
	    }
	  else
	    {
	      APPEND_ALL_DATA(grammar, xmlEntityDeclEvent->publicId, true);
	    }
	}
      else
	grammar->append(AdaptiveStructuralSymbols::EndOfBlock);

      if (xmlEntityDeclEvent->notationName)
	{
	  if (!xmlEntityDeclEvent->notationName[0])
	    {
	      grammar->append(AdaptiveStructuralSymbols::EmptyString);
	      grammar->append(AdaptiveStructuralSymbols::EndOfBlock);
	    }
	  else
	    {
	      APPEND_ALL_DATA(grammar, xmlEntityDeclEvent->notationName, true);
	    }
	}
      else
	grammar->append(AdaptiveStructuralSymbols::EndOfBlock);

      if (xmlEntityDeclEvent->isParameterEntity)
	grammar->append(AdaptiveStructuralSymbols::IsParameterEntityYes);
      else
	grammar->append(AdaptiveStructuralSymbols::IsParameterEntityNo);

      break;


      /**** Notation declaration ****/
    case XmlModelEvent::NotationDecl:
      xmlNotationDeclEvent = (XmlNotationDeclEvent *)event;

      grammar->append(AdaptiveStructuralSymbols::NotationDecl);

      if (xmlNotationDeclEvent->notationName)
	{
	  APPEND_ALL_DATA(grammar, xmlNotationDeclEvent->notationName, true);
	}
      else
	grammar->append(AdaptiveStructuralSymbols::EndOfBlock);

     
      if (xmlNotationDeclEvent->systemId)
	{
	  if (!xmlNotationDeclEvent->systemId[0])
	    {
	      grammar->append(AdaptiveStructuralSymbols::EmptyString);
	      grammar->append(AdaptiveStructuralSymbols::EndOfBlock);
	    }
  	  else
	    {
	      APPEND_ALL_DATA(grammar, xmlNotationDeclEvent->systemId, true);
	    }
	}
      else
	grammar->append(AdaptiveStructuralSymbols::EndOfBlock);

      if (xmlNotationDeclEvent->publicId)
	{
	  if (!xmlNotationDeclEvent->publicId[0])
	    {
	      grammar->append(AdaptiveStructuralSymbols::EmptyString);
	      grammar->append(AdaptiveStructuralSymbols::EndOfBlock);
	    }
	  else
	    {
	      APPEND_ALL_DATA(grammar, xmlNotationDeclEvent->publicId, true);
	    }
	}
      else
	grammar->append(AdaptiveStructuralSymbols::EndOfBlock);


      break;


      /**** Start of element ****/
    case XmlModelEvent::StartElement:
      startElementEvent = (XmlStartElementEvent *)event;

      //end the eventual character data section
      if (inCharacters)
 	{
 	  END_CHARACTER_DATA_SECTION(grammar);
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


	  NEW(ec, AdaptiveElementContext);

	  //set unique element id
	  ec->name = elName;

	  NEW(ec->modeler, ElementModeler(elName));

	  NEW(emi, ElementModelerInfo);
	  emi->modeler = ec->modeler;

	  //new elements always build
	  emi->building = true;

     	  DBG("NEW ELEMENT: " << startElementEvent->name);

	  DBG("Element has this id: " << elementId);
	  if (elementModelerStack.isEmpty())
	    {

	      //first element processed
	      elementModelerStack.push(emi);
	      emi->modeler->resetCurrentNode();
	      clueEdge = 1;

	      //append "new element" symbol to the grammar
	      grammar->append(AdaptiveStructuralSymbols::NewElement);
	      //append the name of the element to the grammar
	      APPEND_ALL_DATA(grammar, startElementEvent->name, true);
	    }
	  else
	    {
	      //move to the new element
	      result = elementModelerStack.top()->modeler->moveToDesiredNode(ElementModel::ElementNode, &edge, &elts, emi->modeler);

	      if (result == ElementModel::NewNodeCreated)
		{
		  if (elementModelerStack.top()->building)
		    {
		      DBG("BUILDING: New node created");

		      //append "new element" symbol to the grammar
		      grammar->append(AdaptiveStructuralSymbols::NewElement);
		      //append the name of the element to the grammar
		      APPEND_ALL_DATA(grammar, startElementEvent->name, true);
		      RESET_CLUE_EDGE;
		    }
		  else
		    {
		      DBG("NACK: New node created");
		      REPORT_NACK;
		      RESET_CLUE_EDGE;

		      //nack + new node created --> we're building
		      elementModelerStack.top()->building = true;

		      //append "new element" symbol to the grammar
		      grammar->append(AdaptiveStructuralSymbols::NewElement);
		      //append the name of the element to the grammar
		      APPEND_ALL_DATA(grammar, startElementEvent->name, true);
		    }
		}
	      else
		if (result == ElementModel::Indefinite)
		  {
		    //prediction failed --> use different edge
		    DBG("NACK NEW: indefinite - add code");
		    REPORT_NACK;
		    REPORT_DIFFERENT_EDGE(edge);

		    RESET_CLUE_EDGE;
		    clueEdge = 1;
		  }
		else
		  {
		    // 		    DBG("NEW definite - add code");
		  }
	      elementModelerStack.push(emi);
	      ec->modeler->resetCurrentNode();
	      clueEdge = 1;
	    }

	  elementModelerList.append(ec->modeler);

	  //add new element context to the element hashtable
	  elements->insert(elementId, ec);


	  //update the element counter
	  elementCounter++;
	}
      else
	{
	  //known element started
	  //--> update its element context
	  ec = elements->find(*elId);
	  CHECK_POINTER(ec);

  	  DBG("KNOWN: " << ec->name);
	  DBG("KNOWN ID: " << *elId);

	  NEW(emi, ElementModelerInfo);
	  emi->modeler = ec->modeler;
	  //known elements use prediction
	  emi->building = false;

	  emi->modeler->increaseRefCount();

	  elementId = *elId;


	  result = elementModelerStack.top()->modeler->moveToDesiredNode(ElementModel::ElementNode, &edge, &elts, emi->modeler);
	  
	  if (result == ElementModel::NewNodeCreated)
	    {
	      if (elementModelerStack.top()->building)
		{
		  DBG("BUILDING: New node created");

		  grammar->append(AdaptiveStructuralSymbols::KnownElement);
		  fibItems = Fibonacci::encodeToBuffer(fibBuf, SIZEOF_XML_CHAR, elementId);
		  APPEND_BUFFER_DATA(grammar, fibBuf, fibItems);
		  RESET_CLUE_EDGE;
		}
	      else
		{
		  DBG("NACK: New node created");
		  REPORT_NACK;
		  RESET_CLUE_EDGE;
		  //nack + new node created --> we're building
		  elementModelerStack.top()->building = true;

		  grammar->append(AdaptiveStructuralSymbols::KnownElement);
		  fibItems = Fibonacci::encodeToBuffer(fibBuf, SIZEOF_XML_CHAR, elementId);
		  APPEND_BUFFER_DATA(grammar, fibBuf, fibItems);
		}
	    }
	  else
	    if (result == ElementModel::Indefinite)
	      {
		//prediction failed --> use different edge

		DBG("NACK KNOWN indefinite - add code");
		REPORT_NACK;
		REPORT_DIFFERENT_EDGE(edge);

		RESET_CLUE_EDGE;
		clueEdge = 1;
		clueEdgeReset = true;
	      }
	    else
	      {
		DBG("KNOWN definite");
 		clueEdge++;
	      }
	  
	  

	  elementModelerStack.push(emi);
	  ec->modeler->resetCurrentNode();

	  DBG("clue edge = " << clueEdge);
	}

      //push element name to the stack
      elementStack->push(new unsigned long(elementId));



      if (startElementEvent->attr[0])
	{
	  DBG("element has attributes");
	  
	  if (emi->modeler->setAttributes(true))
	    {
	      DBG("... as supposed");
	      RESET_CLUE_EDGE;
	    }
	  else
	    {
	      DBG("... violated!");
	      REPORT_NACK;
	      RESET_CLUE_EDGE;
	    }

	  //here is the code that writes down the attributes
       	  for (size_t i = 0; startElementEvent->attr[i]; i += 2)
       	    {
       	      if (!(attrId = attributeIds->find((XmlChar *)startElementEvent->attr[i])))
       		{
       		  //unknown attribute started
       		  //--> we'll create new element context for it


       		  NEW(attrName, XmlChar[xmlchar_strlen(startElementEvent->attr[i]) + 1]);
       		  xmlchar_strcpy(attrName, startElementEvent->attr[i]);
       		  attributeNamesList->append(attrName);
      


       		  //insert new element id to elementIds
       		  attributeId = attributeCounter;
       		  attributeIds->insert(attrName, new unsigned long(attributeId));

		  DBG("INSERTING ATTRIBUTE " << attributeId << ": " << attrName);
		  attributeNames->insert(attributeId, attrName);


       		  //update the element counter
       		  attributeCounter++;


       		  //append "new element" symbol to the grammar
       		  grammar->append(AdaptiveStructuralSymbols::NewElement);

       		  //append the name of the element to the grammar
		  //ending zero  included
       		  APPEND_ALL_DATA(grammar, attrName, true);
       		}
       	      else
       		{
       		  //known attribute started
       		  //--> update its element context
       		  attributeId = *attrId;

       		  //append "known element" symbol to the grammar
       		  grammar->append(AdaptiveStructuralSymbols::KnownElement);
       		  fibItems = Fibonacci::encodeToBuffer(fibBuf, SIZEOF_XML_CHAR, attributeId);
       		  APPEND_BUFFER_DATA(grammar, fibBuf, fibItems);
       		}

       	      //append the attribute value
	      //no zero value at the end of the data block
       	      if (startElementEvent->attr[i+1][0] == 0)
       		grammar->append(AdaptiveStructuralSymbols::EmptyString);
       	      else
       		APPEND_ALL_DATA(grammar, startElementEvent->attr[i+1], false);
       	    }

	  //append the "end of block" symbol to indicate the end of the attribute section
	  grammar->append(AdaptiveStructuralSymbols::EndOfBlock);
	}
      else
	{
	  DBG("element hasn't attributes");

	  if (!emi->modeler->setAttributes(false))
	    {
	      DBG("... as supposed");
	      clueEdge++;
	    }
	  else
	    {
	      DBG("... violated!");
	      REPORT_NACK;
	      RESET_CLUE_EDGE;
	    }
	}

      DBG("clue edge = " << clueEdge);

      break;



      /**** End of element ****/
    case XmlModelEvent::EndElement:
      endElementEvent = (XmlEndElementEvent *)event;

      //end the eventual character data section
      if (inCharacters)
 	{
 	  END_CHARACTER_DATA_SECTION(grammar);
 	}

      //AdaptiveElementContext *ec;

      if (!(elId = elementIds->find((XmlChar *)endElementEvent->name)))
	{
	  //end of unknown element occurred
	  //--> should never happen
	  FATAL("End of unknown element acurred: " << (const char *)endElementEvent->name);
	}
      else
	{
	  //known element ended

	  // 	  DBG("  Known element " << (const char *)endElementEvent->name << " ended");
	}

      elementStack->pop();

      DBG("ELEMENT ENDED: " << endElementEvent->name);

      result = elementModelerStack.top()->modeler->moveToDesiredNode(ElementModel::EndNode, &edge, &elts);

      if (result == ElementModel::NewNodeCreated)
	{
	  if (elementModelerStack.top()->building)
	    {
	      DBG("BUILDING: New node created (end node)");

	      RESET_CLUE_EDGE;

	      grammar->append(AdaptiveStructuralSymbols::EndElement);
	    }
	  else
	    {
	      DBG("NACK: New node created (end node)");
	      REPORT_NACK;

	      RESET_CLUE_EDGE;

	      grammar->append(AdaptiveStructuralSymbols::EndElement);
	    }
	}
      else
	if (result == ElementModel::Indefinite)
	  {
	    //prediction failed --> use different edge
	    DBG("NACK END: indefinite - add code");
	    REPORT_NACK;
	    REPORT_DIFFERENT_EDGE(edge);

	    RESET_CLUE_EDGE;
	  }
	else
	  {
	    //  	    DBG("END definite - add code");
	    INCREASE_CLUE_EDGE;
	  }



      elementModelerStack.pop();

      if (!elementModelerStack.isEmpty())
	{
	  elementModelerStack.top()->modeler->popCurrentNode();
	}
      break;



      /**** Character data ****/
    case XmlModelEvent::Characters:
      charactersEvent = (XmlCharactersEvent *)event;

      if (!inCharacters)
	{
	  inCharacters = true;

	  result = elementModelerStack.top()->modeler->moveToDesiredNode(ElementModel::CharactersNode, &edge, &elts);
      
	  if (result == ElementModel::NewNodeCreated)
	    {
	      if (elementModelerStack.top()->building)
		{
		  DBG("BUILDING: New character node created");
	      
		  RESET_CLUE_EDGE;
	      
		}
	      else
		{
		  DBG("NACK: New node created");
		  DBG("unexpected characters");
		  REPORT_NACK;
	      
		  RESET_CLUE_EDGE;
	      
		  //nack + new node created --> we're building
		  elementModelerStack.top()->building = true;
	      
		}
	    }
	  else
	    if (result == ElementModel::Indefinite)
	      {
		//prediction failed --> use different edge
	    
		DBG("NACK KNOWN indefinite - add code");
		REPORT_NACK;
		REPORT_DIFFERENT_EDGE(edge);
	    
		RESET_CLUE_EDGE;
	      }
	    else
	      {
		DBG("CHARACTERS definite - add code");
		RESET_CLUE_EDGE;
	      }

	  grammar->append(AdaptiveStructuralSymbols::Characters);
	  RESET_CLUE_EDGE;

	}

      //append the character data without the ending 0 (that will be appended at another time)
      APPEND_ALL_DATA_LENGTH(grammar, charactersEvent->data, charactersEvent->length, false);
	  
      break;

      /**** Default data ****/
    case XmlModelEvent::Default:
      defaultEvent = (XmlDefaultEvent *)event;

      if (elementModelerStack.isEmpty())
	{
	  //append "default" symbol to the grammar
	  grammar->append(AdaptiveStructuralSymbols::Default);
	  APPEND_ALL_DATA_LENGTH(grammar, defaultEvent->data, defaultEvent->length, true);
	}
      else
	{
	  if (!inCharacters)
	    {
	      inCharacters = true;

	      result = elementModelerStack.top()->modeler->moveToDesiredNode(ElementModel::CharactersNode, &edge, &elts);
	      // 	  INCREASE_CLUE_EDGE;
      
	      if (result == ElementModel::NewNodeCreated)
		{
		  if (elementModelerStack.top()->building)
		    {
		      DBG("BUILDING: New character node created (default)");
	      
		      RESET_CLUE_EDGE;
	      
		    }
		  else
		    {
		      DBG("NACK: New node created (default)");
		      REPORT_NACK;
	      
		      RESET_CLUE_EDGE;
	      
		      //nack + new node created --> we're building
		      elementModelerStack.top()->building = true;
	      
		    } 
		}
	      else
		if (result == ElementModel::Indefinite)
		  {
		    //prediction failed --> use different edge
	    
		    DBG("NACK KNOWN indefinite - add code (default)");
		    REPORT_NACK;
		    REPORT_DIFFERENT_EDGE(edge);
	    
		    RESET_CLUE_EDGE;
		  }
		else
		  {
		    //  		DBG("KNOWN definite - add code");
		    RESET_CLUE_EDGE;
		  }

	      grammar->append(AdaptiveStructuralSymbols::Characters);
	      RESET_CLUE_EDGE;

	    }
	

	  //grammar->append(AdaptiveStructuralSymbols::Default);
	  APPEND_ALL_DATA_LENGTH_CONVERT_AMP(grammar, defaultEvent->data, defaultEvent->length, false);
	}
      break;


      /**** Comment ****/
    case XmlModelEvent::Comment:
      commentEvent = (XmlCommentEvent *)event;

      if (elementModelerStack.isEmpty())
	{
	  //comments before the root element are reported via the default handler
	  grammar->append(AdaptiveStructuralSymbols::Default);
	}
      else
	{
	  if (!inCharacters)
	    {
	      inCharacters = true;

	      result = elementModelerStack.top()->modeler->moveToDesiredNode(ElementModel::CharactersNode, &edge, &elts);
	      // 	  INCREASE_CLUE_EDGE;
      
	      if (result == ElementModel::NewNodeCreated)
		{
		  if (elementModelerStack.top()->building)
		    {
		      DBG("BUILDING: New character node created (comment)");
	      
		      RESET_CLUE_EDGE;
	      
		    }
		  else
		    {
		      DBG("NACK: New node created (comment)");
		      REPORT_NACK;
	      
		      RESET_CLUE_EDGE;
	      
		      //nack + new node created --> we're building
		      elementModelerStack.top()->building = true;
	      
		    }
		}
	      else
		if (result == ElementModel::Indefinite)
		  {
		    //prediction failed --> use different edge
	    
		    DBG("NACK KNOWN indefinite - add code (comment)");
		    REPORT_NACK;
		    REPORT_DIFFERENT_EDGE(edge);
	    
		    RESET_CLUE_EDGE;
		  }
		else
		  {
		    //  		DBG("KNOWN definite - add code");
		    RESET_CLUE_EDGE;
		  }

	      grammar->append(AdaptiveStructuralSymbols::Characters);
	      RESET_CLUE_EDGE;

	    }
	}


      //append "comment" symbol to the grammar
      grammar->append(AdaptiveStructuralSymbols::Comment);

      APPEND_ALL_DATA(grammar, commentEvent->data, true);
      break;


      /**** StartCDATA ****/
    case XmlModelEvent::StartCDATA:

      if (!inCharacters)
	{
	  inCharacters = true;

	  result = elementModelerStack.top()->modeler->moveToDesiredNode(ElementModel::CharactersNode, &edge, &elts);
	  INCREASE_CLUE_EDGE;
      
	  if (result == ElementModel::NewNodeCreated)
	    {
	      if (elementModelerStack.top()->building)
		{
		  DBG("BUILDING: New character node created (CDATA)");
	      
		  RESET_CLUE_EDGE;
		}
	      else
		{
		  DBG("NACK: New node created (CDATA)");
		  REPORT_NACK;
	      
		  RESET_CLUE_EDGE;
	      
		  //nack + new node created --> we're building
		  elementModelerStack.top()->building = true;
	      
		}
	    }
	  else
	    if (result == ElementModel::Indefinite)
	      {
		//prediction failed --> use different edge
	    
		DBG("NACK KNOWN indefinite - add code (CDATA)");
		REPORT_NACK;
		REPORT_DIFFERENT_EDGE(edge);
	    
		RESET_CLUE_EDGE;
	      }
	    else
	      {
		//  		DBG("KNOWN definite - add code");
	      }

	  grammar->append(AdaptiveStructuralSymbols::Characters);
	  RESET_CLUE_EDGE;

	}



      //append "CDATA" symbol to the grammar
      grammar->append(AdaptiveStructuralSymbols::CDATA);
      break;

      /**** EndCDATA ****/
    case XmlModelEvent::EndCDATA:
      //endCDATAEvent = (XmlEndCDATAEvent *)event;

      //append "comment" symbol to the grammar
      grammar->append(AdaptiveStructuralSymbols::EndOfBlock);
      break;



      /**** Processing instruction ****/
    case XmlModelEvent::PI:
      piEvent = (XmlPIEvent *)event;


      if (elementModelerStack.isEmpty())
	{
	  //PIs before the root element are reported via the default handler
	  grammar->append(AdaptiveStructuralSymbols::Default);
	}
      else
	{


	  if (!inCharacters)
	    {
	      inCharacters = true;

	      result = elementModelerStack.top()->modeler->moveToDesiredNode(ElementModel::CharactersNode, &edge, &elts);
	      // 	  INCREASE_CLUE_EDGE;
      
	      if (result == ElementModel::NewNodeCreated)
		{
		  if (elementModelerStack.top()->building)
		    {
		      DBG("BUILDING: New character node created (PI)");
	      
		      RESET_CLUE_EDGE;
		    }
		  else
		    {
		      DBG("NACK: New node created (PI)");
		      DBG("unexpected PI");
		      REPORT_NACK;
	      
		      RESET_CLUE_EDGE;
	      
		      //nack + new node created --> we're building
		      elementModelerStack.top()->building = true;
		    }
		}
	      else
		if (result == ElementModel::Indefinite)
		  {
		    //prediction failed --> use different edge
	    
		    DBG("NACK KNOWN indefinite (PI)");
		    REPORT_NACK;
		    REPORT_DIFFERENT_EDGE(edge);
	    
		    RESET_CLUE_EDGE;
		  }
		else
		  {
		    //  		DBG("KNOWN definite - add code");
		    RESET_CLUE_EDGE;
		  }

	      grammar->append(AdaptiveStructuralSymbols::Characters);
	      RESET_CLUE_EDGE;

	    }
	}

      //append "pi" symbol to the grammar
      grammar->append(AdaptiveStructuralSymbols::PI);

      if (piEvent->target)
	{
	  APPEND_ALL_DATA(grammar, piEvent->target, true);

	  if (piEvent->data)
	    {
	      APPEND_ALL_DATA(grammar, piEvent->data, true);
	    }
	  else
	    grammar->append(AdaptiveStructuralSymbols::EndOfBlock);
	}
      else
	grammar->append(AdaptiveStructuralSymbols::EndOfBlock);
      
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
void XmlAdaptiveModel::receiveData(XmlChar *data, size_t size)
{
  bool stateChanged = false;
  DataQueueItem *dataQueueItem;
  AdaptiveElementContext *ec;
  XmlChar *elName;
  unsigned long elementId;
  unsigned long *elId;

  //some variables for working with Fibonacci codes
  unsigned char fibTmpChar, fibMask;
  bool fibFinished = false;

  size_t edge;
  size_t differentEdge;
  size_t elts = 0;

  if (!buffer)
    {
      NEW(buffer, XmlChar[XML_MODEL_BUFFER_DEFAULT_SIZE]);
      RESET_BUFFER;
    }

  for (size_t i = 0; i < size; i++)
    {
      if (state != AdaptiveStructuralSymbols::KnownElement)
	{
	  switch (data[i])
	    {
	    case AdaptiveStructuralSymbols::Characters:
	      DBG("Changing to characters, state = " << state);
	      if (state != AdaptiveStructuralSymbols::Characters)
		{
		  CHANGE_STATE(AdaptiveStructuralSymbols::Characters);
		  DBG("Changed to characters");

		}
	      else
		stateChanged = true;

	      if (stateChanged)
		{
		  DBG("1");
		  FINISH_START_ELEMENT_WITH_NO_ATTRIBUTES;

		  //if we are not reading the attribute values, move in the models
		  if (!inAttr)
		    {
		      attrPos = 1;
		      bool willAppend = true;
		      //go as far as we can and then build a new character node - if needed
		      if (!readingNackData && !nackEndedInCharacters)
			{
			  DBG("CHARACTERS --> moving forward");
			  ElementModel::NodeType res = ElementModel::ElementNode;				
			  ElementModel::Node *node = 0;					
								
			  while (res != ElementModel::NoNode)				
			    {								
			      DBG("before moving forward:");
//  			      elementModelerStack.top()->modeler->print();
			      res = elementModelerStack.top()->modeler->moveForward(&node, saxEmitter, userData);	
			      DBG("after moving forward:");
//  			      elementModelerStack.top()->modeler->print();
								
			      //moving over the element's end?
			      if (res == ElementModel::EndNode)
				{
				  DBG("moving over the element's end --> performing pop()");
				  elementModelerStack.pop();
				  elementModelerStack.top()->modeler->popCurrentNode();
				}
			      else
				//jumped to other element model				
				if (res == ElementModel::ElementNode)			
				  {							
				    ElementModelerInfo *eminfo;				
				    NEW(eminfo, ElementModelerInfo);			
				    eminfo->modeler = node->modeler;				
				    eminfo->building = false;				
	  							
				    //DBG("%%%%%%%%%%%%%%%%% 3");

				    SAFE_CALL_EMITTER(startElement(userData, eminfo->modeler->getElementName(), 0));
				    eminfo->modeler->setAttributes(false);

				    elementModelerStack.push(eminfo);			
				    eminfo->modeler->resetCurrentNode();
				  }		
				else
				  if (res == ElementModel::CharactersNode)
				    {
				      DBG("CHARACTERS NODE FOUND!");
				      willAppend = false;
				      break;
				    }

			    }
			}
		      else
			{
			  readingNackData = false;
			  inNACK = false;
			}

		      if (willAppend && !nackEndedInCharacters)
			{
			  DBG("Appending characters node:");
			  elementModelerStack.top()->modeler->moveToDesiredNode(ElementModel::CharactersNode, &edge, &elts);
			  DBG("After appending:");
//  			  elementModelerStack.top()->modeler->print();
			}

		      if (nackEndedInCharacters)
			nackEndedInCharacters = false;


		      continue;
		    }


		}
	      break;

	      //also end of block(ignored) and differentedge!
	    case AdaptiveStructuralSymbols::NACK:
	      if (!inCharacters && !inProlog)
		{
		  DBG("changing to NACK/different edge");
		  DBG((int)data[i]);
		  DBG(inNACK);
		      
		  CHANGE_STATE(AdaptiveStructuralSymbols::NACK);
		  if (stateChanged)
		    {
		      DBG("changed to NACK/differentedge");
		      continue;
		    }

		  DBG("not changed to NACK/differentedge");
		  break;
		}


	      //also used as XmlDecl!
	    case AdaptiveStructuralSymbols::NewElement:

	      CHANGE_STATE(AdaptiveStructuralSymbols::NewElement);
	      if (stateChanged)
		{
		  if (!inAttr)
		    {
		      FINISH_START_ELEMENT_WITH_NO_ATTRIBUTES;
		    }
		  continue;
		}
	      break;

	    case AdaptiveStructuralSymbols::KnownElement:
	      //DBG("KNOWN ELEMENT");

	      CHANGE_STATE(AdaptiveStructuralSymbols::KnownElement);
	      if (stateChanged)
		{
		  if (!inAttr)
		    {
		      FINISH_START_ELEMENT_WITH_NO_ATTRIBUTES;
		    }
		  continue;
		}
	      break;

	    case AdaptiveStructuralSymbols::EndElement:
	      if (inProlog)
		{
		  if (!inDoctype)
		    {
		      DBG("doctype starting");
		      CHANGE_STATE(AdaptiveStructuralSymbols::EndElement);
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
		      DBG("doctype ended");
		      RESET_STATE;
		      RESET_BUFFER;
		      continue;
		    }
		}
	      else
		{
		  DBG("element ended");
		  CHANGE_STATE(AdaptiveStructuralSymbols::None);

		  if (stateChanged)
		    {
		      DBG("4");
		      FINISH_START_ELEMENT_WITH_NO_ATTRIBUTES;

		      elId = elementStack->pop();
		      CHECK_POINTER(elId);
		      ec = elements->find(*elId);

	      

		      /*****/
		      //go as far as we can and then build a new element node
		      if (!readingNackData)
			{
			  DBG("END --> moving forward");
			  ElementModel::NodeType res = ElementModel::ElementNode;				
			  ElementModel::Node *node = 0;					
								
			  while (res != ElementModel::NoNode)				
			    {								
			      DBG("before moving forward:");
//  			      elementModelerStack.top()->modeler->print();
			      res = elementModelerStack.top()->modeler->moveForward(&node, saxEmitter, userData);	
			      DBG("after moving forward:");
//  			      elementModelerStack.top()->modeler->print();
								
			      //moving over the element's end?
			      if (res == ElementModel::EndNode)
				{
				  DBG("moving over the element's end --> performing pop()");
				  elementModelerStack.pop();
				  elementModelerStack.top()->modeler->popCurrentNode();
				}
			      else
				/*jumped to other element model*/				
				if (res == ElementModel::ElementNode)			
				  {							
				    ElementModelerInfo *eminfo;				
				    NEW(eminfo, ElementModelerInfo);			
				    eminfo->modeler = node->modeler;				
				    eminfo->building = false;				
	  							
				    elementModelerStack.push(eminfo);			
				    eminfo->modeler->resetCurrentNode();
				  }							
			    }
			}
		      else
			{
			  readingNackData = false;
			  inNACK = false;
			}

		      DBG("Appending end element node:");
		      elementModelerStack.top()->modeler->moveToDesiredNode(ElementModel::EndNode, &edge, &elts);
		      DBG("After appending:");
//  		      elementModelerStack.top()->modeler->print();

		      SAFE_CALL_EMITTER(endElement(userData, elementModelerStack.top()->modeler->getElementName()));


		      DBG("moving over the element's end --> performing pop()");
		      elementModelerStack.pop();
		      if (!elementModelerStack.isEmpty())
			{
			  elementModelerStack.top()->modeler->popCurrentNode();
			}

		      DBG("Top of the stack (" << elementModelerStack.count() << "):");
		      if (elementModelerStack.top())
			{
//  			  elementModelerStack.top()->modeler->print();
			}
		      else
			{
			  DBG(" stack empty");
			}
		      /******/

		      continue;
		    }
		  else
		    {
		      //DBG("!!!!!!!!! " << state);
		      //DBG((unsigned int)data[i]);
		    }
		}

	      break;

	    case AdaptiveStructuralSymbols::Default:
	      //DBG("COMMENT");
	      CHANGE_STATE(AdaptiveStructuralSymbols::Default);
	      if (stateChanged)
		{
		  DBG("5 default...");
		  FINISH_START_ELEMENT_WITH_NO_ATTRIBUTES;
		  continue;
		}
	      break;


	    case AdaptiveStructuralSymbols::EntityDecl:
	      //DBG("ENTITY DECL");
	      CHANGE_STATE(AdaptiveStructuralSymbols::EntityDecl);
	      if (stateChanged)
		{
		  continue;
		}
	      break;

	    case AdaptiveStructuralSymbols::NotationDecl:
	      //DBG("NOTATION DECL");
	      CHANGE_STATE(AdaptiveStructuralSymbols::NotationDecl);
	      if (stateChanged)
		{
		  continue;
		}
	      break;

	    default:
	      if (inAttrValue)
		{
		  DBG("attribute value");
		  state = AdaptiveStructuralSymbols::Characters;
		}
	    }
	}

      /***************************************/

      
      switch (state)
	{
	  //used as Doctype!
	case AdaptiveStructuralSymbols::EndElement:
	  if (dataQueue->count() == 3)
	    {
	      XmlChar *doctype, *publicId, *systemId;

	      doctype = dataQueue->dequeue()->data;
	      publicId = dataQueue->dequeue()->data;
	      systemId = dataQueue->dequeue()->data;

	      SAFE_CALL_EMITTER(startDoctypeDecl(userData, doctype, systemId, publicId,
						 data[0] == AdaptiveStructuralSymbols::HasInternalSubsetYes));

	      if (doctype)
		{
		  DBG(doctype);
		  DELETE(doctype);
		}

	      if (publicId)
		{
		  DBG(publicId);
		  DELETE(publicId);
		}

	      if (systemId)
		{
		  DBG(systemId);
		  DELETE(systemId);
		}

	      DBG("doctype start complete");
	      RESET_BUFFER;
	      RESET_STATE;
	    }
	  else
	    if (data[i] == AdaptiveStructuralSymbols::EndOfBlock)
	      {
		//read target value
		NEW(dataQueueItem, DataQueueItem);
		dataQueueItem->type = Characters;

		if (bufferLength)
		  {
		    if (buffer[0] == AdaptiveStructuralSymbols::EmptyString)
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

	case AdaptiveStructuralSymbols::NACK:
	  if (!inNACK)
	    {
	      DBG("NACK data");

	      fibMask = 1 << (SIZEOF_CHAR*8 - 1);
	      
	      //decode symbol using Fibonacci code
	      fibTmpChar = (unsigned char)data[i];
	      DBG("SSS: " << (unsigned int)fibTmpChar);

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
			  clueEdge = Fibonacci::decode(fibCode);
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
		  inNACK = true;

		  DBG("############# clue edge: " << clueEdge);
 		  readingNackData = true;

		  //complete Fibonacci code read and decoded
		  //reset fibCode, and fibBits
		  fibCode = 0;
		  fibBits = 0;
		  fibLastWasOne = false;

		  RESET_BUFFER;
		  RESET_STATE;

		  /*****/
		  //move clueEdge-1 edges forward
		  DBG("NACK --> moving");

		  ElementModel::NodeType res = ElementModel::ElementNode;				
		  ElementModel::Node *node = 0;					

		  if (elementModelerStack.top()->modeler)
		    {
		      DBG("!before moving forward!:");
//  		      elementModelerStack.top()->modeler->print();
		    }
								

		  {
		    bool moved = false;
		    while (clueEdge > 1)				
		      {
			DBG("CURRENT VALUE OF CLUE EDGE: " << clueEdge);

			DBG("before moving forward XX:");
// 			elementModelerStack.top()->modeler->print();
			  
			if (attrPos == 0)
			  {
			    DBG("We are entering the attribute section");

			    attrPos = 1;
			    clueEdge--;

			    if (clueEdge == 1)
			      {
				DBG("skipped the attribute node, because the element has no attributes");
				dataQueueItem = dataQueue->dequeue();
				      
				//emit startElement event with no attributes
				//FINISH_START_ELEMENT_WITH_NO_ATTRIBUTES;
				SAFE_CALL_EMITTER(startElement(userData, dataQueueItem->data, 0));
				elementModelerStack.top()->modeler->setAttributes(false);	//zz
			      }
			    continue;

			  }
			else
			  {
			    //we are not in the attribute section

			    {
			      DBG("before moving forward:");
// 			      elementModelerStack.top()->modeler->print();
			      res = elementModelerStack.top()->modeler->moveForward(&node, saxEmitter, userData);	
			      DBG("after moving forward:");
// 			      elementModelerStack.top()->modeler->print();


			      //patch the element with no attributes and subsequent deterministic other things
			      if (attrPos == 1 && !moved)
				{
				  DBG("DATA QUEUE COUNT: " << dataQueue->count());

				  if (!dataQueue->isEmpty())
				    {
				      dataQueueItem = dataQueue->dequeue();
					
				      //emit startElement event with no attributes
				      //FINISH_START_ELEMENT_WITH_NO_ATTRIBUTES;
				      SAFE_CALL_EMITTER(startElement(userData, dataQueueItem->data, 0));
				      elementModelerStack.top()->modeler->setAttributes(false);	//zzz
				    }
				}

			      //moving over the element's end?
			      if (res == ElementModel::EndNode)
				{
				  DBG("moving over the element's end --> performing pop()");
				  elementModelerStack.pop();
				  elementModelerStack.top()->modeler->popCurrentNode();
				}
			      else
				/*jumped to other element model*/				
				if (res == ElementModel::ElementNode)			
				  {							
				    ElementModelerInfo *eminfo;				
				    NEW(eminfo, ElementModelerInfo);			
				    eminfo->modeler = node->modeler;				
				    eminfo->building = false;				
				      
				    elementModelerStack.push(eminfo);			
				    eminfo->modeler->resetCurrentNode();

				    DBG("enqueueing the element name");
				    NEW(dataQueueItem, DataQueueItem);
				    dataQueueItem->type = Characters;
				    NEW(dataQueueItem->data, XmlChar[xmlchar_strlen(eminfo->modeler->getElementName())+1]);
				    xmlchar_strcpy(dataQueueItem->data, eminfo->modeler->getElementName());
				    dataQueue->enqueue(dataQueueItem);

				    attrPos = 0;
				  }
			
			      moved = true;
			      clueEdge--;
			    }
			  }

			DBG("current location:");
// 			elementModelerStack.top()->modeler->print();
		      }

		    if (attrPos == 0)
		      {
			DBG("We ended in the attribute section");

			if (elementModelerStack.top()->modeler->hasAttributes())
			  {
			    DBG("Element should have the attributes... and HAS NOT");
			    attrPos = 1;	//we left the attribute node
				
			    elementModelerStack.top()->modeler->setAttributes(false);
			    DBG("element name : " << elementModelerStack.top()->modeler->getElementName());
			    DBG("the next prediction will be " << elementModelerStack.top()->modeler->hasAttributes());
				
			    DBG("DATA QUEUE COUNT: " << dataQueue->count());
				

			    dataQueueItem = dataQueue->dequeue();
			    //emit startElement event with no attributes
			    SAFE_CALL_EMITTER(startElement(userData, dataQueueItem->data, 0));
			    //FINISH_START_ELEMENT_WITH_NO_ATTRIBUTES;

			    inAttr = false;
			    inAttrValue = false;
			    readingNackData = false;
			    inNACK = false;
			  }
			else
			  {
			    DBG("Element should NOT have the attributes... and HAS");
				
			    attrPos = 0;	//we entered the attribute node
				
			    //the attribute stats will be updated in characters when reading the attributes!

			    DBG("element name : " << elementModelerStack.top()->modeler->getElementName());
			    DBG("the next prediction will be " << elementModelerStack.top()->modeler->hasAttributes());
				
			    DBG("DATA QUEUE COUNT: " << dataQueue->count());
				
			    if (dataQueue->isEmpty())
			      {
				FATAL("no element name present!");
			      }
			    else
			      {
				DBG("DATA QUEUE COUNT: " << dataQueue->count());
			      }


			    inAttr = true;
			    inAttrValue = false;
			    readingNackData = false;
			    inNACK = false;
			  }
			    
		      }
		    /******/
		      
		  }
		}
	    }
	  //different edge data
	  else
	    {
	      DBG("DifferentEdge data");

	      fibMask = 1 << (SIZEOF_CHAR*8 - 1);
	      
	      //decode symbol using Fibonacci code
	      fibTmpChar = (unsigned char)data[i];

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
			  differentEdge = Fibonacci::decode(fibCode);
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
		  DBG("############# different edge: " << differentEdge);
		  readingNackData = false;
		  inNACK = false;

		  //complete Fibonacci code read and decoded
		  //reset fibCode, and fibBits
		  fibCode = 0;
		  fibBits = 0;
		  fibLastWasOne = false;

		  RESET_BUFFER;
		  RESET_STATE;

		  //clue edge is now the chosen edge
		  clueEdge = 1;


		  /*****/
		  //move clueEdge-1 edges forward
		  DBG("DIFFERENT EDGE --> moving");
		  {
		    ElementModel::NodeType res = ElementModel::ElementNode;				
		    ElementModel::Node *node = 0;					
								
		    DBG("before moving forward:");
//  		    elementModelerStack.top()->modeler->print();
		    res = elementModelerStack.top()->modeler->followEdge(differentEdge, &node, saxEmitter, userData);	
		    DBG("after moving forward:");
//  		    elementModelerStack.top()->modeler->print();
								
		    //moving over the element's end?
		    if (res == ElementModel::EndNode)
		      {
			DBG("moving over the element's end --> performing pop()");
			//SAFE_CALL_EMITTER(endElement(userData, elementModelerStack.top()->modeler->getElementName()));
			elementModelerStack.pop();
			elementModelerStack.top()->modeler->popCurrentNode();
		      }
		    else
		      /*jumped to other element model*/				
		      if (res == ElementModel::ElementNode)			
			{							
			  dataQueue->clear();	//!!! workaround for element with no attrs

			  ElementModelerInfo *eminfo;				
			  NEW(eminfo, ElementModelerInfo);			
			  eminfo->modeler = node->modeler;				
			  eminfo->building = false;				
		      

			  elementModelerStack.push(eminfo);			
			  eminfo->modeler->resetCurrentNode();

			  DBG("enqueueing the element name");
			  NEW(dataQueueItem, DataQueueItem);
			  dataQueueItem->type = Characters;
			  NEW(dataQueueItem->data, XmlChar[xmlchar_strlen(eminfo->modeler->getElementName())+1]);
			  xmlchar_strcpy(dataQueueItem->data, eminfo->modeler->getElementName());
			  dataQueue->enqueue(dataQueueItem);
			  
			  //we are in the attribute section
			  attrPos = 0;


			  if (!eminfo->modeler->hasAttributes())
			    {
			      //the element has no atributes (prediction, NACK can follow!)
			      //--> move throught the fictive attribute node
			      
			      inAttr = false;
			      DBG("DE: Element shouldn't have attributes");
			    }
			  else
			    {
			      //the element has attributes
			      //--> jump into the fictive attribute node
			      inAttr = true;
			      DBG("DE: Element should have attributes");
			    }

			}		

		    DBG("current location:");
//  		    elementModelerStack.top()->modeler->print();

		    if (elementModelerStack.top()->modeler->currentNodeType() == ElementModel::CharactersNode)
		      {
			//we ended in the characters node
			//CHANGE_STATE(AdaptiveStructuralSymbols::Characters);
			DBG("$$$$$$$$$$$$$$$$$$$$$$$");
			nackEndedInCharacters = true;
		      }
		  }
		  /******/

		}
	    }

	  break;


	case AdaptiveStructuralSymbols::NewElement:
	  inProlog = false;

	  if (data[i] == AdaptiveStructuralSymbols::EndOfBlock)
	    {
	      //decoding new element/attribute

	      //add ending \0
	      buffer[bufferLength] = 0;
	      bufferLength++;


	      if (inAttr)
		{
		  //we are in the attribute section --> read the attribute name and value
		  DBG("NEW attribute " << buffer);
		  inAttrValue = true;

		  XmlChar *attrName;
		  NEW(attrName, XmlChar[xmlchar_strlen(buffer)+1]);
		  xmlchar_strcpy(attrName, buffer);

		  DBG("Attribute has this id: " << attributeCounter);
		  attributeNamesList->append(attrName);
		  attributeNames->insert(attributeCounter, attrName);

		  attributeNames->insert(attributeCounter, attrName);
		  attributeCounter++;


		  NEW(dataQueueItem, DataQueueItem);
		  dataQueueItem->type = Characters;
		  NEW(dataQueueItem->data, XmlChar[xmlchar_strlen(buffer)+1]);
		  xmlchar_strcpy(dataQueueItem->data, buffer);
		  dataQueue->enqueue(dataQueueItem);
		  
		  RESET_STATE;
		  RESET_BUFFER;
		  continue;
		}
	      else
		{
		  //move forward until:
		  // 1. attribute section --> inAttr = true OR
		  // 2. new node for new element
		  DBG("NEW element " << buffer);

		  //insert the name of the element onto the stack
		  NEW(dataQueueItem, DataQueueItem);
		  dataQueueItem->type = Characters;
		  NEW(dataQueueItem->data, XmlChar[xmlchar_strlen(buffer)+1]);
		  xmlchar_strcpy(dataQueueItem->data, buffer);
		  dataQueue->enqueue(dataQueueItem);


		  ElementModelerInfo *emi;

		  if (elementModelerStack.isEmpty())
		    {
		      //first element processed


		      //save element name
		      NEW(elName, XmlChar[bufferLength]);
		      xmlchar_strcpy(elName, buffer);
		      elementNamesList->append(elName);
		      
		      //create an  element contex for the new element
		      NEW(ec, AdaptiveElementContext);
		      
		      //set unique element id
		      ec->name = elName;
		      
		      elementId = elementCounter;

		      DBG("NEW ELEMENT HAS THIS ID: " << elementId);
		      
		      //add new element context to the element hashtable
		      elements->insert(elementCounter, ec);
		      
		      //update the element counter
		      elementCounter++;
		      
		      elementStack->push(new unsigned long(elementId));
		      
		      //reset buffer
		      RESET_BUFFER;
		      
		      RESET_STATE;
		      
		      
		      NEW(ec->modeler, ElementModeler(elName));
		      elementModelerList.append(ec->modeler);

		      
		      NEW(emi, ElementModelerInfo);
		      emi->modeler = ec->modeler;
		      
		      //new elements always build
		      emi->building = true;
		      

		      DBG("initiating the root model");
		      elementModelerStack.push(emi);
		      DBG("items on the stack: " << elementModelerStack.count());
		      emi->modeler->resetCurrentNode();
		      clueEdge = 0;

		      if (!emi->modeler->hasAttributes())
			{
			  //the element has no atributes (prediction, NACK can follow!)
			  //--> move throught the fictive attribute node
			  
			  inAttr = false;
			  DBG("1Element shouldn't have attributes");
			  attrPos = 0;
			}
		      else
			{
			  //the element has attributes
			  //--> jump into the fictive attribute node
			  inAttr = true;
			  DBG("1Element should have attributes");
			  attrPos = 0;
			}

		    }
		  else
		    {
		      //move to the new element

		      /******/
		      //go as far as we can and then build a new element node
		      //only if not reading the nack information !
		      if (!readingNackData)
			{
			  DBG("NEW ELEMENT --> moving");


			  ElementModel::NodeType res = ElementModel::ElementNode;				
			  ElementModel::Node *node = 0;					
			  ElementModel::Node *cr;
			  bool moved = false;
				
			  bool leave = false;
			  while (res != ElementModel::NoNode)				
			    {				

			      //patch the first step
			      cr = elementModelerStack.top()->modeler->getCurrentNode();
			      if (moved && cr->type == ElementModel::StartNode && elementModelerStack.top()->modeler->hasAttributes())
				{
				  DBG("We should stop in the attribute section!");
				  DBG("Current element: " << elementModelerStack.top()->modeler->getElementName());
				  inAttr = true;
				  inAttrValue = true;
				  moved = false;

				  //we have to flip the attribute name and the name of the element (:-)
				  dataQueueItem = dataQueue->dequeue();
				  XmlChar *attrName = dataQueueItem->data;

				  //enqueue the name of the element
				  NEW(dataQueueItem, DataQueueItem);
				  NEW(dataQueueItem->data, XmlChar[xmlchar_strlen(elementModelerStack.top()->modeler->getElementName())+1]);
				  xmlchar_strcpy(dataQueueItem->data, elementModelerStack.top()->modeler->getElementName());
				  dataQueueItem->type = Characters;
				  dataQueue->enqueue(dataQueueItem);


				  //enqueue the name of the attribute
				  NEW(dataQueueItem, DataQueueItem);
				  dataQueueItem->type = Characters;
				  dataQueueItem->data = attrName;
				  dataQueue->enqueue(dataQueueItem);


				  RESET_STATE;
				  RESET_BUFFER;

				  leave = true;
				  break;

				}

			      attrPos = 1;	//at this point we are always after the attribute node

			      moved = true;
			      DBG("before moving forward:");
//  			      elementModelerStack.top()->modeler->print();
			      res = elementModelerStack.top()->modeler->moveForward(&node, saxEmitter, userData);	
			      DBG("after moving forward:");
//  			      elementModelerStack.top()->modeler->print();
								
			      //moving over the element's end?
			      if (res == ElementModel::EndNode)
				{
				  DBG("moving over the element's end --> performing pop()");
				  elementModelerStack.pop();
				  elementModelerStack.top()->modeler->popCurrentNode();
				}
			      else
				/*jumped to other element model*/				
				if (res == ElementModel::ElementNode)			
				  {							
				    ElementModelerInfo *eminfo;				
				    NEW(eminfo, ElementModelerInfo);			
				    eminfo->modeler = node->modeler;				
				    eminfo->building = false;				

				    elementModelerStack.push(eminfo);
				    eminfo->modeler->resetCurrentNode();

				    if (!eminfo->modeler->hasAttributes())
				      {
					//the element has no atributes (prediction, NACK can follow!)
					//--> move throught the fictive attribute node

					inAttr = false;
					attrPos = 1; 	//move behind the attribute section

					XmlChar *tmp;
					NEW(tmp, XmlChar[xmlchar_strlen(eminfo->modeler->getElementName())+1]);
					xmlchar_strcpy(tmp, eminfo->modeler->getElementName());
			    
					//emit startElement event with no attributes
					SAFE_CALL_EMITTER(startElement(userData, tmp, 0));
					eminfo->modeler->setAttributes(false);
				      }
				    else
				      {
					//the element has attributes
					//--> jump into the fictive attribute node
					inAttr = true;
					DBG("2Element should have attributes");
					attrPos = 0;
				      }
				  }							
			    }

			  if (leave)
			    continue;

			  //save element name
			  NEW(elName, XmlChar[bufferLength]);
			  xmlchar_strcpy(elName, buffer);
			  elementNamesList->append(elName);
			  
			  //DBG("!!!!!!!!! " << elName);
			  //create an  element contex for the new element
			  NEW(ec, AdaptiveElementContext);
		      
			  //set unique element id
			  ec->name = elName;
			  
			  elementId = elementCounter;
			  
			  DBG("NEW ELEMENT HAS THIS ID: " << elementId);
			  //add new element context to the element hashtable
			  elements->insert(elementCounter, ec);
			  
			  //update the element counter
			  elementCounter++;
			  
			  elementStack->push(new unsigned long(elementId));
			  
			  //reset buffer
			  RESET_BUFFER;
			  
			  RESET_STATE;
		      
		      
			  NEW(ec->modeler, ElementModeler(elName));
			  elementModelerList.append(ec->modeler);
		      
			  NEW(emi, ElementModelerInfo);
			  emi->modeler = ec->modeler;
		      
			  //new elements always build
			  emi->building = true;

			}
		      else
			{
			  readingNackData = false;
			  inNACK = false;


			  //save element name
			  NEW(elName, XmlChar[bufferLength]);
			  xmlchar_strcpy(elName, buffer);
			  elementNamesList->append(elName);
			  
			  //create an  element contex for the new element
			  NEW(ec, AdaptiveElementContext);
		      
			  //set unique element id
			  ec->name = elName;
			  
			  elementId = elementCounter;
			 
			  DBG("NEW ELEMENT HAS THIS ID: " << elementId);
 
			  //add new element context to the element hashtable
			  elements->insert(elementCounter, ec);
			  
			  //update the element counter
			  elementCounter++;
			  
			  elementStack->push(new unsigned long(elementId));
			  
			  //reset buffer
			  RESET_BUFFER;
			  
			  RESET_STATE;
		      
		      
			  NEW(ec->modeler, ElementModeler(elName));
			  elementModelerList.append(ec->modeler);
		      
			  NEW(emi, ElementModelerInfo);
			  emi->modeler = ec->modeler;
		      
			  //new elements always build
			  emi->building = true;

			}



		      DBG("Appending new model:");
//  		      emi->modeler->print();

		      elementModelerStack.top()->modeler->moveToDesiredNode(ElementModel::ElementNode, &edge, &elts, emi->modeler);
		      DBG("After appending:");
//  		      elementModelerStack.top()->modeler->print();


		      DBG("Appended model:");
		      emi->modeler->resetCurrentNode();
//  		      emi->modeler->print();

		      elementModelerStack.push(emi);			
		      DBG("items on the stack: " << elementModelerStack.count());
		      DBG("Top of the stack:");
//  		      elementModelerStack.top()->modeler->print();

		      if (!emi->modeler->hasAttributes())
			{
			  //the element has no atributes (prediction, NACK can follow!)
			  //--> move throught the fictive attribute node
			  
			  inAttr = false;
			  DBG("3Element shouldn't have attributes");
			  attrPos = 0;

			}
		      else
			{
			  //the element has attributes
			  //--> jump into the fictive attribute node
			  inAttr = true;
			  DBG("3Element should have attributes");
			  attrPos = 0;
			}

		    }



		}


	    }
	  else
	    {
	      //read next character of the incomplete element name
	      buffer[bufferLength] = data[i];
	      bufferLength++;
	    }
	  break;

	  //also used as XML declaration!
	case AdaptiveStructuralSymbols::KnownElement:
	  if (inProlog)
	    {
	      //reading the XML declaration

	      if (data[i] == AdaptiveStructuralSymbols::StandaloneYes || data[i] == AdaptiveStructuralSymbols::StandaloneNo
		  || (data[i] == AdaptiveStructuralSymbols::StandaloneNotSpecified && dataQueue->count() == 2))
		{
		  //read standalone information
		  XmlChar *version;
		  XmlChar *encoding;
		  int standalone;

		  switch(data[i])
		    {
		    case AdaptiveStructuralSymbols::StandaloneYes:
		      standalone = 1;
		      break;

		    case AdaptiveStructuralSymbols::StandaloneNo:
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
		if (data[i] == AdaptiveStructuralSymbols::EndOfBlock)
		  {
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

	    }
	  else
	    {

	      //known element started
	      //--> read complete Fibonacci code of its id

	      //construct reverse Fibonacci code (e.g. 1100101 instead of 1010011) from sequence of
	      //following bytes
	      fibMask = 1 << (SIZEOF_CHAR*8 - 1);
	      
	      //decode symbol using Fibonacci code
	      fibTmpChar = (unsigned char)data[i];

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
		  //complete Fibonacci code read and decoded
		  //reset fibCode, and fibBits
		  fibCode = 0;
		  fibBits = 0;
		  fibLastWasOne = false;

		  RESET_BUFFER;
		  RESET_STATE;



		  if (inAttr)
		    {
		      //we are in the attribute section --> read the attribute name and value
		      DBG("KNOWN attribute " << elementId);
		      inAttrValue = true;

		      //get the name of the attribute from the attribute names hashtable
		      XmlChar *attrName = attributeNames->find(elementId);
		      CHECK_POINTER(attrName);

 		      NEW(dataQueueItem, DataQueueItem);
 		      dataQueueItem->type = Characters;
 		      NEW(dataQueueItem->data, XmlChar[xmlchar_strlen(attrName)+1]);
 		      xmlchar_strcpy(dataQueueItem->data, attrName);
 		      dataQueue->enqueue(dataQueueItem);
		      
		      continue;
		    }
		  else
		    {
		      //we read the id of the known element

		      //dataQueue->clear();	//!!! WORKAROUND because of the startelement with no attributes!
		      FINISH_START_ELEMENT_WITH_NO_ATTRIBUTES;
		      ec = elements->find(elementId);

		      DBG("KNOWN ELEMENT WITH THIS ID: " << elementId);

		      CHECK_POINTER(ec);


		      elementStack->push(new unsigned long(elementId));

		      DBG("DATA QUEUE COUNT: " << dataQueue->count());

		      //insert the name of the element onto the stack
		      NEW(dataQueueItem, DataQueueItem);
		      dataQueueItem->type = Characters;
		      NEW(dataQueueItem->data, XmlChar[xmlchar_strlen(ec->name)+1]);
		      xmlchar_strcpy(dataQueueItem->data, ec->name);
		      dataQueue->enqueue(dataQueueItem);
		      DBG("Enqueued the name of the element:" << ec->name);

		      ElementModelerInfo *emi;
		      NEW(emi, ElementModelerInfo);
		      emi->modeler = ec->modeler;

		      /******/
		      //go as far as we can and then build a new element node
		      //only if not reading the nack information !
		      if (!readingNackData)
			{
			  DBG("KNOWN ELEMENT " << ec->name << "--> moving");

			  ElementModel::NodeType res = ElementModel::ElementNode;				
			  ElementModel::Node *node = 0;					


			  ElementModel::Node *cr;
			  bool moved = false;
			  bool leave = false;

			  while (res != ElementModel::NoNode)				
			    {				
			      //patch the first step
			      cr = elementModelerStack.top()->modeler->getCurrentNode();
			      if (moved && cr->type == ElementModel::StartNode && elementModelerStack.top()->modeler->hasAttributes())
				{
				  DBG("We should stop in the attribute section!");
				  DBG("Current element: " << elementModelerStack.top()->modeler->getElementName());
				  inAttr = true;
				  inAttrValue = true;
				  moved = false;

				  //we have to flip the attribute name and the name of the element (:-)
				  dataQueueItem = dataQueue->dequeue();
				  XmlChar *attrName = dataQueueItem->data;

				  //enqueue the name of the element
				  NEW(dataQueueItem, DataQueueItem);
				  NEW(dataQueueItem->data, XmlChar[xmlchar_strlen(elementModelerStack.top()->modeler->getElementName())+1]);
				  xmlchar_strcpy(dataQueueItem->data, elementModelerStack.top()->modeler->getElementName());
				  dataQueueItem->type = Characters;
				  dataQueue->enqueue(dataQueueItem);


				  //enqueue the name of the attribute
				  NEW(dataQueueItem, DataQueueItem);
				  dataQueueItem->type = Characters;
				  dataQueueItem->data = attrName;
				  dataQueue->enqueue(dataQueueItem);


				  RESET_STATE;
				  RESET_BUFFER;

				  leave = true;
				  break;

				}

			      attrPos = 1;	//at this point we are always after the attribute node

			      moved = true;




			      DBG("before moving forward:");
//  			      elementModelerStack.top()->modeler->print();
			      res = elementModelerStack.top()->modeler->moveForward(&node, saxEmitter, userData);	
			      DBG("after moving forward:");
//  			      elementModelerStack.top()->modeler->print();
			
			      //moving over the element's end?
			      if (res == ElementModel::EndNode)
				{
				  DBG("moving over the element's end --> performing pop()");
				  elementModelerStack.pop();
				  elementModelerStack.top()->modeler->popCurrentNode();
				}
			      else
				//jumped to other element model?
				if (res == ElementModel::ElementNode)			
				  {							
				    ElementModelerInfo *eminfo;				
				    NEW(eminfo, ElementModelerInfo);			
				    eminfo->modeler = node->modeler;				
				    eminfo->building = false;				
			      
				    elementModelerStack.push(eminfo);			
				    eminfo->modeler->resetCurrentNode();


				    if (!eminfo->modeler->hasAttributes())
				      {
					//the element has no atributes (prediction, NACK can follow!)
					//--> move throught the fictive attribute node
				    
					inAttr = false;

					attrPos = 1; 	//move behind the attribute section

					XmlChar *tmp;
					NEW(tmp, XmlChar[xmlchar_strlen(eminfo->modeler->getElementName())+1]);
					xmlchar_strcpy(tmp, eminfo->modeler->getElementName());
			    
					//emit startElement event with no attributes
					SAFE_CALL_EMITTER(startElement(userData, tmp, 0));
					eminfo->modeler->setAttributes(false);

				      }
				    else
				      {
					//the element has attributes
					//--> jump into the fictive attribute node
					inAttr = true;
					DBG("4Element should have attributes");
					attrPos = 0;
				      }
				
				  }					
			    }
			  if (leave)
			    continue;
			}
		      else
			{
			  readingNackData = false;
			  inNACK = false;
			}




		      DBG("Appending new model:");
//  		      emi->modeler->print();

		      emi->modeler->increaseRefCount();
	      
		      elementModelerStack.top()->modeler->moveToDesiredNode(ElementModel::ElementNode, &edge, &elts, emi->modeler);
		      DBG("After appending:");
//  		      elementModelerStack.top()->modeler->print();

		      DBG("Appended model:");
		      emi->modeler->resetCurrentNode();
//  		      emi->modeler->print();
	      
		      elementModelerStack.push(emi);			
		      DBG("items on the stack: " << elementModelerStack.count());
		      DBG("Top of the stack:");
//  		      elementModelerStack.top()->modeler->print();


		      if (!emi->modeler->hasAttributes())
			{
			  //the element has no atributes (prediction, NACK can follow!)
			  //--> move throught the fictive attribute node
			  
			  inAttr = false;
			  DBG("5Element shouldn't have attributes");
			  attrPos = 0;
			  DBG("DATA QUEUE COUNT: " << dataQueue->count());
			}
		      else
			{
			  //the element has attributes
			  //--> jump into the fictive attribute node
			  inAttr = true;
			  DBG("5Element should have attributes");
			  attrPos = 0;
			}
		    }

		}
	    }

	  break;

	case AdaptiveStructuralSymbols::Default:
	  //default data
	  if (data[i] == AdaptiveStructuralSymbols::EndOfBlock)
	    {

	      if (inComment)
		{
		  //emit the comment data
		  inComment = false;

		  if (bufferLength)
		    //send data only if they aren't empty
		    SAFE_CALL_EMITTER(comment(userData, buffer));

		  RESET_BUFFER;

		  DBG("COMMENT ENDED");
		}
	      else

		//CDATA is not allowed to occur before the root element!
		// 		if (inCDATA)
		// 		  {
		// 		    //emit the CDATA data
		// 		    inCDATA = false;

		// 		    if (bufferLength)
		// 		      //send data only if they aren't empty
		// 		      SAFE_CALL_EMITTER(characterData(userData, buffer, bufferLength));
			
		// 		    SAFE_CALL_EMITTER(endCDATASection(userData));
		// 		    RESET_BUFFER;
		// 		    DBG("CDATA ENDED");
		// 		  }
		// 		else
		if (inPI)
		  {
		    inPI--;

		    buffer[bufferLength] = data[i];
		    bufferLength++;
		    //complete PI read
		    if (!inPI)
		      {
			//emit the PI
			DBG("PI ENDED");

			size_t k = 0;

			while (buffer[k] != AdaptiveStructuralSymbols::EndOfBlock)
			  k++;

			DBG("K = " << k << ", bufferLength = " << bufferLength);
			if (k < bufferLength)
			  {
			    //both target and value
			    SAFE_CALL_EMITTER(processingInstruction(userData, buffer, buffer+k+1));
			  }
			else
			  {
			    //no value
			    SAFE_CALL_EMITTER(processingInstruction(userData, buffer, 0));
			  }

			RESET_BUFFER;
		      }
		  }
		else
		  {
		    //emit the default data
		    SAFE_CALL_EMITTER(defaultHandler(userData, buffer, bufferLength));
		    RESET_STATE;
		    RESET_BUFFER;
		  }
	    }
	  else
	    {
	      switch (data[i])
		{
		case AdaptiveStructuralSymbols::Comment:
		  DBG("COMMENT");
		  inComment = true;
		  
		  //if there are any default data in the buffer, send them and reset the buffer
		  if (bufferLength)
		    //send data only if they aren't empty
		    SAFE_CALL_EMITTER(defaultHandler(userData, buffer, bufferLength));
		  
		  RESET_BUFFER;
		  break;
		  
		case AdaptiveStructuralSymbols::PI:
		  DBG("PROCESSING INSTRUCTION");
		  inPI = 2;
		  
		  //if there are any default data in the buffer, send them and reset the buffer
		  if (bufferLength)
		    //send data only if they aren't empty
		    SAFE_CALL_EMITTER(defaultHandler(userData, buffer, bufferLength));
		  
		  RESET_BUFFER;
		  break;

		  //CDATA is not allowed to occur before the root element
		  // 		case AdaptiveStructuralSymbols::CDATA:
		  // 		  DBG("CDATA");
		  // 		  inCDATA = true;
		  
		  // 		  //if there are any character data in the buffer, send them and reset the buffer
		  // 		  if (bufferLength)
		  // 		    //send data only if they aren't empty
		  // 		    SAFE_CALL_EMITTER(characterData(userData, buffer, bufferLength));
		  
		  // 		  RESET_BUFFER;
		  
		  // 		  SAFE_CALL_EMITTER(startCDATASection(userData));
		  // 		  break;

		default:
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
	    }

	  break;


	case AdaptiveStructuralSymbols::EntityDecl:
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

	      SAFE_CALL_EMITTER(entityDecl(userData, entityName, data[0] == AdaptiveStructuralSymbols::IsParameterEntityYes,
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
	    if (data[i] == AdaptiveStructuralSymbols::EndOfBlock)
	      {
		//read target value
		NEW(dataQueueItem, DataQueueItem);
		dataQueueItem->type = Characters;

		if (bufferLength)
		  {
		    if (buffer[0] == AdaptiveStructuralSymbols::EmptyString)
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

	case AdaptiveStructuralSymbols::NotationDecl:
	  if (data[i] == AdaptiveStructuralSymbols::EndOfBlock)
	    {
	      //read target value
	      NEW(dataQueueItem, DataQueueItem);
	      dataQueueItem->type = Characters;
	      
	      if (bufferLength)
		{
		  if (buffer[0] == AdaptiveStructuralSymbols::EmptyString)
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

	case AdaptiveStructuralSymbols::Characters:
	  DBG("reading char " << (unsigned int)data[i]);
	  //characters
	  if (data[i] == AdaptiveStructuralSymbols::EmptyString)
	    {
	      //ignored
	    }
	  else
	    if (inAttrValue)
	      {
		if (data[i] == AdaptiveStructuralSymbols::EndOfBlock ||
		    data[i] == AdaptiveStructuralSymbols::NewElement ||
		    data[i] == AdaptiveStructuralSymbols::KnownElement)
		  
		  {
		    //read the complete value of an attribute
		    inAttrValue = false;

		    if (!firstAttrRead)
		      {
			firstAttrRead = true;

			//update the attribute stats
			elementModelerStack.top()->modeler->setAttributes(true);
		      }

		    buffer[bufferLength] = 0;
		    bufferLength++;
		    DBG("value of the attribute: " << buffer);

		    

		    NEW(dataQueueItem, DataQueueItem);
		    dataQueueItem->type = Characters;
		    NEW(dataQueueItem->data, XmlChar[xmlchar_strlen(buffer)+1]);
		    xmlchar_strcpy(dataQueueItem->data, buffer);
		    dataQueue->enqueue(dataQueueItem);

		    //we read the last attribute
		    if (data[i] == AdaptiveStructuralSymbols::EndOfBlock)
		      {
			inAttr = false;
			firstAttrRead = false;
			attrPos = 1;	//we left the attribute node
			DBG("last attribute value read");
			RESET_STATE;

			DBG("length of the data queue: " << dataQueue->count());
			
			const XmlChar **attrs;				
			const XmlChar *elementName;
			size_t attrPos = 0;							
			
			//dequeue the name of the element
			dataQueueItem = dataQueue->dequeue();
			elementName = dataQueueItem->data;

			//get the attributes
			NEW(attrs, const XmlChar *[dataQueue->count()+1]);			
			attrs[dataQueue->count()] = 0;						
			/* attributes and the element name decoded */				
			while ((dataQueueItem = dataQueue->dequeue()))				
			  {									
			    /* an attribute name or an attribute value */			
			    attrs[attrPos] = dataQueueItem->data;				
			    attrPos++;								
			  }									
			
			//emit the startElement event
			SAFE_CALL_EMITTER(startElement(userData, elementName, attrs));

			DBG("-----");
		      }
		    else
		      {
			//allow the change to newelement/knownelement
			//in order to read the other attributes
			if (data[i] == AdaptiveStructuralSymbols::NewElement)
			  state = AdaptiveStructuralSymbols::NewElement;
			else
			  state = AdaptiveStructuralSymbols::KnownElement;
		      }
		    RESET_BUFFER;
		  }
		else
		  {
		    //read the characters of the attribute value
		    DBG("attrval char: " << data[i]);
		    buffer[bufferLength] = data[i];
		    bufferLength++;
		  }
	      }
	    else

	      if (data[i] == AdaptiveStructuralSymbols::EndOfBlock)
		{
		  if (inComment)
		    {
		      //emit the comment data
		      inComment = false;

		      if (bufferLength)
			//send data only if they aren't empty
			SAFE_CALL_EMITTER(comment(userData, buffer));

		      RESET_BUFFER;

		      DBG("COMMENT ENDED");
		    }
		  else
		    if (inCDATA)
		      {
			//emit the CDATA data
			inCDATA = false;

			if (bufferLength)
			  //send data only if they aren't empty
			  SAFE_CALL_EMITTER(characterData(userData, buffer, bufferLength));
			
			SAFE_CALL_EMITTER(endCDATASection(userData));
			RESET_BUFFER;
			DBG("CDATA ENDED");
		      }
		    else
		      if (inPI)
			{
			  inPI--;

			  buffer[bufferLength] = data[i];
			  bufferLength++;
			  //complete PI read
			  if (!inPI)
			    {
			      //emit the PI
			      DBG("PI ENDED");

			      size_t k = 0;

			      while (buffer[k] != AdaptiveStructuralSymbols::EndOfBlock)
				k++;

			      DBG("K = " << k << ", bufferLength = " << bufferLength);
			      if (k < bufferLength)
				{
				  //both target and value
				  SAFE_CALL_EMITTER(processingInstruction(userData, buffer, buffer+k+1));
				}
			      else
				{
				  //no value
				  SAFE_CALL_EMITTER(processingInstruction(userData, buffer, 0));
				}

			      RESET_BUFFER;
			    }
			}
		      else
			{
			  //emit the character data and reset the state (character data end)
			  if (bufferLength)
			    //send data only if they aren't empty
			    {
			      buffer[bufferLength] = 0;
			      SAFE_CALL_EMITTER(characterData(userData, buffer, bufferLength));
			    }
			  
			  
			  RESET_STATE;
			  RESET_BUFFER;
			}
		  
		}
	      else
		{
		  DBG("Character: '" << data[i] << "' ASCII: "  << (unsigned int)data[i]);

		  switch (data[i])
		    {
		    case AdaptiveStructuralSymbols::Comment:
		      DBG("COMMENT");
		      inComment = true;

		      //if there are any character data in the buffer, send them and reset the buffer
		      if (bufferLength)
			//send data only if they aren't empty
			SAFE_CALL_EMITTER(characterData(userData, buffer, bufferLength));

		      RESET_BUFFER;
		      break;

		    case AdaptiveStructuralSymbols::PI:
		      DBG("PROCESSING INSTRUCTION");
		      inPI = 2;

		      //if there are any character data in the buffer, send them and reset the buffer
		      if (bufferLength)
			//send data only if they aren't empty
			SAFE_CALL_EMITTER(characterData(userData, buffer, bufferLength));

		      RESET_BUFFER;
		      break;

		    case AdaptiveStructuralSymbols::CDATA:
		      DBG("CDATA");
		      inCDATA = true;

		      //if there are any character data in the buffer, send them and reset the buffer
		      if (bufferLength)
			//send data only if they aren't empty
			SAFE_CALL_EMITTER(characterData(userData, buffer, bufferLength));

		      RESET_BUFFER;

		      SAFE_CALL_EMITTER(startCDATASection(userData));
		      break;

		    default:
		      buffer[bufferLength] = data[i];
		      bufferLength++;
		  
		      if (bufferLength == XML_MODEL_BUFFER_DEFAULT_SIZE)
			{
			  //character data larger than the buffer --> split it
			  if (inComment)
			    {
			      //nothing to do, this should never happen!
			    }

			  SAFE_CALL_EMITTER(characterData(userData, buffer, bufferLength));
			  RESET_BUFFER;
			}
		    }
		}
	  break;

	default:
 	  FATAL("default !!!");
	}
    }
}
