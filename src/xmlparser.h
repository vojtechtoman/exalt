/***************************************************************************
    xmlparser.h  -  Definition of XmlParser class
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
  \file xmlparser.h
  \brief Definition of the XmlParser class.
  
  This file contains definition of the XmlParser class.
*/



#ifdef __GNUG__
# pragma interface
#endif

#ifndef XMLPARSER_H
#define XMLPARSER_H



#include "defs.h"
#include "xmlparserbase.h"
#include "xmlchar.h"
#include "options.h"


//! Size of the buffer for reading XML data in PULL mode.
#define XML_BUFFER_SIZE		4096



/*!
  \brief SAX parser.

  This class is a C++ wrapper around the Expat XML parser. The main task of the parser is (surprisingly) to parse given XML stream and manage all important SAX events. These SAX events are transformed into XML model events (XmlModelEvent) and sent to given XmlModel.
 */
class XmlParser : public XmlParserBase
{
public:
  //! A constructor.
  XmlParser(void);

  //! A constructor.
  XmlParser(const XmlChar *encoding);

  //! A destructor.
  virtual ~XmlParser(void);

  //! Parse XML stream.
  virtual long parse(void) throw (ExaltParserException, ExaltEncodingException, ExaltIOException);


  //! Parse XML data in PUSH mode.
  virtual long parsePush(const char *data, int length, bool isFinal) throw (ExaltParserException, ExaltEncodingException, ExaltIOException);

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
  /*!
    \brief Expat parser structure.

    This is the "heart" of the class, which realizes the main part of the SAX parsing. All SAX events emitted by innerParser are redirected to handlers of the class.
   */
  XmlInnerParser innerParser;

  /*!
    Buffer for XML data.

    Data coming from the XML stream are stored in this buffer. If the size of the data is larger than XML_BUFFER_SIZE, the parsing is done in more blocks.
   */
  char xmlDataBuff[XML_BUFFER_SIZE];

  //! Initialization of the parser with given encoding.
  virtual void initParser(const XmlChar *encoding);

  //! Expose the XML event to XML model.
  void exposeEvent(XmlModelEvent *ev);

  //! Total amount of the character data.
  unsigned long charactersTotal;
};



/*!
  \brief %Context structure used during parsing.
  
  Used mainly to hold the pointer to instance of the parser. The C++ wrapper to Expat is realized with some help of static functions that redirect the SAX events to the parser instance.
*/
struct ParseInfo
{
  //! Pointer to the parser instance.
  XmlParserBase *parser;

  //! Current "depth" in the document.
  unsigned long depth;
};


/*!
  \brief A Structure describing character encoding.
 */
struct EncodingInfo
{
  //! Pointer to a text codec.
  TextCodec *textCodec;

  //! The MIB of the encoding.
  Encodings::MIB mib;
};


#endif //XMLPARSER_H
