/***************************************************************************
    saxemitter.h  -  Definition of SAXEmitter class
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
  \file saxemitter.h
  \brief Definition of SAXEmitter class.
  
  This file contains the definition of SAXEmitter class.
*/

#ifdef __GNUG__
# pragma interface
#endif



#ifndef SAXEMITTER_H
#define SAXEMITTER_H

#include "defs.h"
#include "saxbase.h"
#include "saxreceptor.h"




/*!
  \brief An emitter of SAX events.

  The SAX emitter sends SAX events to registered SAX receptor. For example, if you call the startElement method of the emitter, it calls the startElement method of the receptor.
 */
class SAXEmitter : public SAXBase
{
public:
  //! A constructor.
  SAXEmitter(void);

  //! A constructor.
  SAXEmitter(SAXReceptor *receptor);

  //! Set SAX receptor.
  virtual void setSAXReceptor(SAXReceptor *receptor);


  //! Report parser error messages.
  virtual void reportError(void);

  //! Start element handler.
  virtual void startElement(void *userData,
			    const XmlChar *name,
			    const XmlChar **atts);

  //! End element handler.
  virtual void endElement(void *userData,
			  const XmlChar *name);

  //! Character data handler.
  virtual void characterData(void *userData,
			     const XmlChar *data,
			     int length);

  //! Processing instruction handler.
  virtual void processingInstruction(void *userData,
				     const XmlChar *target,
				     const XmlChar *data);

  //! Comment handler.
  virtual void comment(void *userData,
		       const XmlChar *data);

  //! Start CDATA section handler.
  virtual void startCDATASection(void *userData);


  //! End CDATA section handler.
  virtual void endCDATASection(void *userData);

  //! Default handler.
  virtual void defaultHandler(void *userData,
			      const XmlChar *data,
			      int length);

  //! Unknown encoding handler.
  virtual int unknownEncoding(void *unknownEncodingData,
			      const XmlChar *name,
			      XML_Encoding *info);

  //! XML declaration handler.
  virtual void xmlDecl(void *userData,
		       const XmlChar *version,
		       const XmlChar *encoding,
		       int standalone);

  //! Start doctype handler.
  virtual void startDoctypeDecl(void *userData,
				const XmlChar *doctypeName,
				const XmlChar *systemId,
				const XmlChar *publicId,
				int hasInternalSubset);

  //! End doctype handler.
  virtual void endDoctypeDecl(void *userData);


  //! Entity declaration handler.
  virtual void entityDecl(void *userData,
			  const XmlChar *entityName,
			  int isParameterEntity,
			  const XmlChar *value,
			  int valueLength,
			  const XmlChar *base,
			  const XmlChar *systemId,
			  const XmlChar *publicId,
			  const XmlChar *notationName);

  //! Notation declaration handler.
  virtual void notationDecl(void *userData,
			    const XmlChar *notationName,
			    const XmlChar *base,
			    const XmlChar *systemId,
			    const XmlChar *publicId);


protected:
  //! Pointer to the SAX receptor.
  SAXReceptor *saxReceptor;
};

#endif //SAXEMITTER_H
