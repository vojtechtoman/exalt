/***************************************************************************
    xmlmodelevents.h  -  Definitions of XML model events.
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
  \file xmlmodelevents.h
  \brief Definitions of XML model events.
  
  This file contains definitions of XML model events.
*/

#ifndef XMLMODELEVENTS_H
#define XMLMODELEVENTS_H

#include "defs.h"
#include "xmldefs.h"


/*!
  \brief Structure representing generic XML model event.

  This events are sent to XmlModel by XmlParser. All XML model events must inherit this structure.
 */
struct XmlModelEvent
{
  /*!
    Type of the event.
   */
  enum XmlModelEventType
  {
    //! Start of the element.
    StartElement,

    //! End of the element.
    EndElement,

    //! Character data.
    Characters,

    //! Start of CDATA section.
    StartCDATA,

    //! End of CDATA section.
    EndCDATA,

    //! Commentary.
    Comment,

    //! Processing instruction.
    PI,

    //! Start of doctype.
    StartDoctype,

    //! End of doctype.
    EndDoctype,

    //! XML declaration.
    XmlDecl,

    //! Entity declaration.
    EntityDecl,

    //! Entity declaration.
    NotationDecl,

    //! Default event.
    Default
  };

  /*!
    \brief Type of the event.

    With the knowledge of this type, the XmlModel can overtype the event to correct event type (e.g. XmlStartElementEvent).
   */
  XmlModelEventType type;
};



/*!
  \brief XML event reporting start of the element.
 */
struct XmlStartElementEvent : public XmlModelEvent
{
  /*!
    \brief A constructor.
    \param name The name of the element.
    \param attr An array of pairs [attribute name, value].
   */
  XmlStartElementEvent(const XmlChar *name, const XmlChar **attr)
    : XmlModelEvent()
  {
    type = XmlModelEvent::StartElement;
    this->name = name;
    this->attr = attr;
  }

  //! Name of the element.
  const XmlChar *name;

  //! Array of attributes.
  const XmlChar **attr;
};




/*!
  \brief XML event reporting end of the element.
 */
struct XmlEndElementEvent : public XmlModelEvent
{
  /*!
    \brief A constructor.
    \param name The name of the element.
   */
  XmlEndElementEvent(const XmlChar *name)
    : XmlModelEvent()
  {
    type = XmlModelEvent::EndElement;
    this->name = name;
  }

  //! Name of the element.
  const XmlChar *name;
};



/*!
  \brief XML event reporting character data.
 */
struct XmlCharactersEvent : public XmlModelEvent
{
  /*!
    \brief A constructor.
    \param data The character data.
    \param length The character data length.
   */
  XmlCharactersEvent(const XmlChar *data, int length)
    : XmlModelEvent()
  {
    type = XmlModelEvent::Characters;
    this->data = data;
    this->length = length;
  }

  //! The character data.
  const XmlChar *data;

  //! Length of the data.
  int length;
};


/*!
  \brief XML event reporting default data.
 */
struct XmlDefaultEvent : public XmlModelEvent
{
  /*!
    \brief A constructor.
    \param data The default data.
    \param length The data length.
   */
  XmlDefaultEvent(const XmlChar *data, int length)
    : XmlModelEvent()
  {
    type = XmlModelEvent::Default;
    this->data = data;
    this->length = length;
  }

  //! The character data.
  const XmlChar *data;

  //! Length of the data.
  int length;
};



/*!
  \brief XML event reporting CDATA section start.
 */
struct XmlStartCDATAEvent : public XmlModelEvent
{
  //! A constructor.
  XmlStartCDATAEvent(void)
  {
    type = XmlModelEvent::StartCDATA;
  }
};

/*!
  \brief XML event reporting CDATA section end.
 */
struct XmlEndCDATAEvent : public XmlModelEvent
{
  //! A constructor.
  XmlEndCDATAEvent(void)
  {
    type = XmlModelEvent::EndCDATA;
  }
};


/*!
  \brief XML event reporting procession instruction.
 */
struct XmlPIEvent : public XmlModelEvent
{
  /*!
    \brief A constructor.

    \param target The PI's target.
    \param data The PI's data.
  */
  XmlPIEvent(const XmlChar *target, const XmlChar *data)
    : XmlModelEvent()
  {
    type = XmlModelEvent::PI;
    this->target = target;
    this->data = data;
  }

  //! Target.
  const XmlChar *target;

  //! Data of the instruction.
  const XmlChar *data;
};



/*!
  \brief XML event reporting comments.
 */
struct XmlCommentEvent : public XmlModelEvent
{
  /*!
    \brief A constructor.
    \param data The comment data.
   */
  XmlCommentEvent(const XmlChar *data)
    : XmlModelEvent()
  {
    type = XmlModelEvent::Comment;
    this->data = data;
  }

  //! Data of the comment.
  const XmlChar *data;
};



/*!
  \brief XML event reporting XML declaration.
 */
struct XmlDeclEvent : public XmlModelEvent
{
  /*!
    \brief A constructor.
    \param version The XML version.
    \param encoding The encoding of the document.
    \param standalone Is the document standalone?
   */
  XmlDeclEvent(const XmlChar *version, const XmlChar *encoding, int standalone)
    : XmlModelEvent()
  {
    type = XmlModelEvent::XmlDecl;
    this->version = version;
    this->encoding = encoding;
    this->standalone = standalone;
  }

  //! XML version.
  const XmlChar *version;

  //! Encoding of the document.
  const XmlChar *encoding;

  //! Is XML document standalone?
  int standalone;
};



/*!
  \brief XML event reporting the start of the doctype.
 */
struct XmlStartDoctypeEvent : public XmlModelEvent
{
  /*!
    \brief A constructor.
    \param doctypeName The name of the doctype.
    \param systemId The system id.
    \param publicId The public id.
    \param hasInternalSubset Has the DTD an internal subset?
   */
  XmlStartDoctypeEvent(const XmlChar *doctypeName, const XmlChar *systemId, const XmlChar *publicId, int hasInternalSubset)
    : XmlModelEvent()
  {
    type = XmlModelEvent::StartDoctype;
    this->doctypeName = doctypeName;
    this->systemId = systemId;
    this->publicId = publicId;
    this->hasInternalSubset = hasInternalSubset;
  }

  //! Doctype name.
  const XmlChar *doctypeName;

  //! System Id.
  const XmlChar *systemId;

  //! Public Id.
  const XmlChar *publicId;

  /*!
    \brief Info whether the document has internal subset.

    Value 1 means YES, value 0 means NO, and -1 says that this information wasn't contained in the doctype.
   */
  int hasInternalSubset;
};


/*!
  \brief XML event reporting the end of the doctype.
 */
struct XmlEndDoctypeEvent : public XmlModelEvent
{
  //! A constructor.
  XmlEndDoctypeEvent(void) : XmlModelEvent()
  {
    type = XmlModelEvent::EndDoctype;
  }
};



/*!
  \brief XML event reporting entity declaration.
 */
struct XmlEntityDeclEvent : public XmlModelEvent
{
  /*!
    \brief A constructor.
    \param entityName The name of the entity.
    \param isParameterEntity Is the entity a parameter entity?
    \param value The value of the entity.
    \param valueLength The length of the value.
    \param systemId The system id.
    \param publicId The public id.
    \param notationName The name of the notation.
   */
  XmlEntityDeclEvent(const XmlChar *entityName, int isParameterEntity, const XmlChar *value, int valueLength, /* const XmlChar *base, */
		     const XmlChar *systemId, const XmlChar *publicId, const XmlChar *notationName)
    : XmlModelEvent()
  {
    type = XmlModelEvent::EntityDecl;
    this->entityName = entityName;
    this->isParameterEntity = isParameterEntity;
    this->value = value;
    this->valueLength = valueLength;
    //this->base = base;
    this->systemId = systemId;
    this->publicId = publicId;
    this->notationName = notationName;
  }

  //! Name of the entity.
  const XmlChar *entityName;

  //! Is entity parametric?
  int isParameterEntity;

  //! Value of the entity.
  const XmlChar *value;

  //! Length of the value.
  int valueLength;

  //! System Id.
  const XmlChar *systemId;

  //! Public Id.
  const XmlChar *publicId;

  //! Name of the notation.
  const XmlChar *notationName;
};



/*!
  \brief XML event reporting notation declaration.
 */
struct XmlNotationDeclEvent : public XmlModelEvent
{
  /*!
    \brief A constructor.
    \param notationName The name of the notation.
    \param systemId The system id.
    \param publicId The public id.
   */
  XmlNotationDeclEvent(const XmlChar *notationName, /* const XmlChar *base, */ const XmlChar *systemId, const XmlChar *publicId)
    : XmlModelEvent()
  {
    type = XmlModelEvent::NotationDecl;
    this->notationName = notationName;
    //this->base = base;
    this->systemId = systemId;
    this->publicId = publicId;
  }

  //! Name of the notation.
  const XmlChar *notationName;

//    //! Base name.
//    const XmlChar *base;

  //! System Id.
  const XmlChar *systemId;

  //! Public Id.
  const XmlChar *publicId;
};



#endif //XMLEVENTS_H



