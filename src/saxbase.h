/***************************************************************************
    saxbase.h  -  Definition of Communicative class
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
  \file saxbase.h
  \brief Definition of SAXBase class.
  
  This file contains the definition of SAXBase class.
*/



#ifndef SAXBASE_H
#define SAXBASE_H


#include "defs.h"
#include "xmldefs.h"


/*!
  \brief An abstract predecessor of all SAX related classes.

  Inherit this class whenever you want to use the SAX callback functionality.
 */
class SAXBase
{
public:
  /*!
    \brief Report parser error messages.

    Does nothing.
  */
  virtual void reportError(void) {}

  /*!
    \brief Start element handler.

    Does nothing.

    \param userData Pointer to the user data structure.
    \param name Name of the element.
    \param atts An Array of attributes.
  */
  virtual void startElement(void *userData,
			    const XmlChar *name,
			    const XmlChar **atts) {}

  /*!
    \brief End element handler.

    Does nothing.

    \param userData Pointer to the user data structure.
    \param name Name of the element.
  */
  virtual void endElement(void *userData,
			  const XmlChar *name) {}

  /*!
    \brief Character data handler.

    Does nothing.

    \param userData Pointer to the user data structure.
    \param data Character data.
    \param length Length of the data.
  */
  virtual void characterData(void *userData,
			     const XmlChar *data,
			     int length) {}

  /*!
    \brief Processing instruction handler.

    Does nothing.

    \param userData Pointer to the user data structure.
    \param target Target of the instruction.
    \param data Data of the instruction.
  */
  virtual void processingInstruction(void *userData,
				     const XmlChar *target,
				     const XmlChar *data) {}

  /*!
    \brief Comment handler.

    Does nothing.

    \param userData Pointer to the user data structure.
    \param data The comment data.
  */
  virtual void comment(void *userData,
		       const XmlChar *data) {}

  /*!
    \brief Start CDATA section handler.

    Does nothing.

    \param userData Pointer to the user data structure.
  */
  virtual void startCDATASection(void *userData) {}


  /*!
    \brief End CDATA section handler.

    Does nothing.

    \param userData Pointer to the user data structure.
  */
  virtual void endCDATASection(void *userData) {}

  /*!
    \brief Default handler.

    Does nothing.

    \param userData Pointer to the user data structure.
    \param data The default data.
    \param length The length of the default data.
  */
  virtual void defaultHandler(void *userData,
			      const XmlChar *data,
			      int length) {}

  /*!
    \brief Unknown encoding handler.

    Does nothing.

    \param unknownEncodingData The description of the encoding.
    \param name The name of the encoding
    \param info The Expat's info structure.
  */
  virtual int unknownEncoding(void *unknownEncodingData,
			      const XmlChar *name,
			      XML_Encoding *info) { return 0; }


  /*!
    \brief XML declaration handler.

    Does nothing.

    \param userData Pointer to the user data structure.
    \param version The XML version.
    \param encoding The encoding of the document.
    \param standalone Is the document standalone?
  */
  virtual void xmlDecl(void *userData,
		       const XmlChar *version,
		       const XmlChar *encoding,
		       int standalone) {}

  /*!
    \brief Start doctype handler.

    Does nothing.

    \param userData Pointer to the user data structure.
    \param doctypeName The name of the doctype.
    \param systemId The system id.
    \param publicId The public id.
    \param hasInternalSubset Has the DTD an internal subset?
  */
  virtual void startDoctypeDecl(void *userData,
				const XmlChar *doctypeName,
				const XmlChar *systemId,
				const XmlChar *publicId,
				int hasInternalSubset) {}

  /*!
    \brief End doctype handler.

    Does nothing.

    \param userData Pointer to the user data structure.
  */
  virtual void endDoctypeDecl(void *userData) {}


  /*!
    \brief Entity declaration handler.

    Does nothing.

    \param userData Pointer to the user data structure.
    \param entityName The name of the entity.
    \param isParameterEntity Is the entity a parametric entity?
    \param value The value of the entity.
    \param valueLength The length of the value.
    \param base The base.
    \param systemId The system id.
    \param publicId The public id.
    \param notationName The name of the notation.
  */
  virtual void entityDecl(void *userData,
			  const XmlChar *entityName,
			  int isParameterEntity,
			  const XmlChar *value,
			  int valueLength,
			  const XmlChar *base,
			  const XmlChar *systemId,
			  const XmlChar *publicId,
			  const XmlChar *notationName) {}

  /*!
    \brief Notation declaration handler.

    Does nothing.

    \param userData Pointer to the user data structure.
    \param notationName The name of the notation.
    \param base The base.
    \param systemId The system id.
    \param publicId The public id.
  */
  virtual void notationDecl(void *userData,
			    const XmlChar *notationName,
			    const XmlChar *base,
			    const XmlChar *systemId,
			    const XmlChar *publicId) {}


};

#endif //SAXBASE_H
