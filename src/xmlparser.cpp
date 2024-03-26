/***************************************************************************
    xmlparser.cpp  -  Definitions of class XmlParser methods
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
  \file xmlparser.cpp
  \brief Definition of the XmlParser class methods.
  
  This file contains definition of the XmlParser class methods.
*/


#ifdef __GNUG__
# pragma implementation
#endif


#include "xmlparser.h"



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////



static void startElementHandler_wrapper(void *userData, const XmlChar *el, const XmlChar **attr)
{
  ParseInfo *inf = (ParseInfo *)userData;
  
  inf->parser->startElement(userData, el, attr);
}


static void endElementHandler_wrapper(void *userData, const XmlChar *el)
{
  ParseInfo *inf = (ParseInfo *)userData;
  
  inf->parser->endElement(userData, el);
}


static void characterDataHandler_wrapper(void *userData,
					 const XmlChar *data,
					 int length)
{
  ParseInfo *inf = (ParseInfo *)userData;
  
  inf->parser->characterData(userData, data, length);
}

static void processingInstructionHandler_wrapper(void *userData,
						 const XmlChar *target,
						 const XmlChar *data)
{
  ParseInfo *inf = (ParseInfo *)userData;
  
  inf->parser->processingInstruction(userData, target, data);
}

static void commentHandler_wrapper(void *userData,
				   const XmlChar *data)
{
  ParseInfo *inf = (ParseInfo *)userData;
  
  inf->parser->comment(userData, data);
}

static void startCDATASectionHandler_wrapper(void *userData)
{
  ParseInfo *inf = (ParseInfo *)userData;
  
  inf->parser->startCDATASection(userData);
}


static void endCDATASectionHandler_wrapper(void *userData)
{
  ParseInfo *inf = (ParseInfo *)userData;
  
  inf->parser->endCDATASection(userData);
}

static void defaultHandler_wrapper(void *userData,
				   const XmlChar *data,
				   int length)
{
  ParseInfo *inf = (ParseInfo *)userData;
  
  inf->parser->defaultHandler(userData, data, length);
}

static int unknownEncodingHandler_wrapper(void *unknownEncodingData,
					  const XmlChar *name,
					  XML_Encoding *info)
{
  ParseInfo *inf = (ParseInfo *)unknownEncodingData;
  
  return inf->parser->unknownEncoding(unknownEncodingData, name, info);
}

static int convert_wrapper(void *data, const char *s)
{
  EncodingInfo *inf = (EncodingInfo *)data;

  return inf->textCodec->convert(s, inf->mib);
}

static void release_wrapper(void *data)
{
  EncodingInfo *inf = (EncodingInfo *)data;

  inf->textCodec->release(inf->mib);
}

static void xmlDeclHandler_wrapper(void *userData,
				   const XmlChar *version,
				   const XmlChar *encoding,
				   int standalone)
{
  ParseInfo *inf = (ParseInfo *)userData;
  
  inf->parser->xmlDecl(userData, version, encoding, standalone);
}

static void startDoctypeDeclHandler_wrapper(void *userData,
					    const XmlChar *doctypeName,
					    const XmlChar *systemId,
					    const XmlChar *publicId,
					    int hasInternalSubset)
{
  ParseInfo *inf = (ParseInfo *)userData;
  
  inf->parser->startDoctypeDecl(userData, doctypeName, systemId, publicId, hasInternalSubset);
}

static void endDoctypeDeclHandler_wrapper(void *userData)
{
  ParseInfo *inf = (ParseInfo *)userData;
  
  inf->parser->endDoctypeDecl(userData);
}


static void entityDeclHandler_wrapper(void *userData,
				      const XmlChar *entityName,
				      int isParameterEntity,
				      const XmlChar *value,
				      int valueLength,
				      const XmlChar *base,
				      const XmlChar *systemId,
				      const XmlChar *publicId,
				      const XmlChar *notationName)
{
  ParseInfo *inf = (ParseInfo *)userData;
  
  inf->parser->entityDecl(userData, entityName, isParameterEntity, value, valueLength, base, systemId, publicId, notationName);
}


static void notationDeclHandler_wrapper(void *userData,
					const XmlChar *notationName,
					const XmlChar *base,
					const XmlChar *systemId,
					const XmlChar *publicId)
{
  ParseInfo *inf = (ParseInfo *)userData;
  
  inf->parser->notationDecl(userData, notationName, base, systemId, publicId);
}




//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////



/*!
  Calls initParser().
*/
XmlParser::XmlParser(void)
  : XmlParserBase()
{ 
  initParser(0);
}


/*!
  Calls initParser().
  
  \param encoding The encoding of the XML data.
 */
XmlParser::XmlParser(const XmlChar *encoding)
  : XmlParserBase(encoding)
{
  initParser(encoding);
}


/*!
  Initializes the parser (sets the SAX handlers, etc.)

  \param encoding The encoding of the XML data.
 */
void XmlParser::initParser(const XmlChar *encoding)
{
  //create an expat parser
  innerParser = XML_ParserCreate(encoding);
  CHECK_POINTER(innerParser);


  //set user data
  ParseInfo *parseInfo;
  NEW(parseInfo, ParseInfo);
  parseInfo->parser = this;
  XML_SetUserData(innerParser, parseInfo);

  
  //and now... set all the callbacks
  XML_SetStartElementHandler(innerParser, startElementHandler_wrapper);
  XML_SetEndElementHandler(innerParser, endElementHandler_wrapper);
  XML_SetCharacterDataHandler(innerParser, characterDataHandler_wrapper);
  XML_SetProcessingInstructionHandler(innerParser, processingInstructionHandler_wrapper);
  XML_SetCommentHandler(innerParser, commentHandler_wrapper);
  XML_SetStartCdataSectionHandler(innerParser, startCDATASectionHandler_wrapper);
  XML_SetEndCdataSectionHandler(innerParser, endCDATASectionHandler_wrapper);
  XML_SetDefaultHandler(innerParser, defaultHandler_wrapper);
  XML_SetUnknownEncodingHandler(innerParser, unknownEncodingHandler_wrapper, parseInfo);
  XML_SetXmlDeclHandler(innerParser, xmlDeclHandler_wrapper);
  XML_SetStartDoctypeDeclHandler(innerParser, startDoctypeDeclHandler_wrapper);
  XML_SetEndDoctypeDeclHandler(innerParser, endDoctypeDeclHandler_wrapper);
  XML_SetEntityDeclHandler(innerParser, entityDeclHandler_wrapper);
  XML_SetNotationDeclHandler(innerParser, notationDeclHandler_wrapper);
//    XML_SetExternalEntityRefHandler(innerParser, externalEntityRefHandler_wrapper);
//    XML_SetStartNamespaceDeclHandler(innerParser, startNamespaceDeclHandler_wrapper);
//    XML_SetEndNamespaceDeclHandler(innerParser, endNamespaceDeclHandler_wrapper);
//    XML_SetElementDeclHandler(innerParser, elementDeclHandler_wrapper);
//    XML_SetAttlistDeclHandler(innerParser, attlistDeclHandler_wrapper);
//    XML_SetNotStandaloneHandler(innerParser, notStandaloneHandler_wrapper);


  //disable parameter entity parsing
  XML_SetParamEntityParsing(innerParser, XML_PARAM_ENTITY_PARSING_NEVER);

  charactersTotal = 0;
}



/*!
  Frees the resources consumed by the parser.
 */
XmlParser::~XmlParser(void)
{
  //in verbose mode, display the statistics
  if (ExaltOptions::getOption(ExaltOptions::Verbose) == ExaltOptions::Yes)
    {
      OUTPUTENL("Input document statistics");
      OUTPUTENL("  Character data size: \t\t\t" << charactersTotal);
    }

  XML_ParserFree(innerParser);
  deleteDefaultTextCodec();
}



/*!
  Reads the data from the input device and parses it.

  \return The number of characters parsed so far.
 */
long XmlParser::parse(void) throw (ExaltParserException, ExaltEncodingException, ExaltIOException)
{
  bool final;


  if (!inputDevice)
    {
      FATAL("No input device has been specified!");
    }
  else
    {
      while (inputDevice->readData(xmlDataBuff, XML_BUFFER_SIZE) == ReadOk)
	{
	  final = inputDevice->bytesRead() < XML_BUFFER_SIZE;
	  
	  if (!XML_Parse(innerParser, xmlDataBuff, inputDevice->bytesRead(), final))
	    {
	      reportError();
	      return 0;
	    }
	  else
	    if (final)
	      {
		return XML_GetCurrentByteIndex(innerParser);
	      }
	  
	}
      
      if (inputDevice->errorOccurred())
	FATAL("Error reading from input device!");
      
      if (!XML_Parse(innerParser, xmlDataBuff, inputDevice->bytesRead(), true))
	{
	  reportError();
	  return 0;
	}
      else
	{
	  return XML_GetCurrentByteIndex(innerParser);
	}
    }

  return 0;
}



/*!
  Parses given data in the PUSH mode.

  \param data Buffer of XMl data.
  \param length The length of the buffer.
  \param isFinal Indication if the buffer contains the last portion of XML data.
 */
long XmlParser::parsePush(const char *data, int length, bool isFinal)
  throw (ExaltParserException, ExaltEncodingException, ExaltIOException)
{
  if (!XML_Parse(innerParser, data, length, isFinal))
    {
      reportError();
      return 0;
    }
  else
    if (isFinal)
      {
	return XML_GetCurrentByteIndex(innerParser);
      }

  return 0;
}





//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////



/*!
  Used for reporting error messages generated by Expat.
 */
void XmlParser::reportError(void)
{
  PARSERERR("Line " << XML_GetCurrentLineNumber(innerParser) <<		\
	    ", column " << XML_GetCurrentColumnNumber(innerParser) <<	\
	    ": " << XML_ErrorString(XML_GetErrorCode(innerParser)));

  throw ExaltParseErrorException();
}



/*!
  Examines the name of the element and the values of its attributes and emits corresponding events to the XML model.

  \param userData Pointer to the user data structure.
  \param name Name of the element.
  \param attr An array of attributes.

  \sa XmlParser::exposeEvent()
 */
void XmlParser::startElement(void *userData, const XmlChar *name, const XmlChar **attr)
{
  ParseInfo *parseInfo = (ParseInfo *)userData;

  parseInfo->depth++;

#ifdef PRINT_SAX
  XmlChar *ee;
  NEW(ee, XmlChar[xmlchar_strlen(name)+10]);

  xmlchar_strcpy(ee, "<");
  xmlchar_strcat(ee, name);
  xmlchar_strcat(ee, ">");

  OUTPUT(ee);

  DELETE_ARRAY(ee);

  if (attr)
    if (attr[0])
      {
	OUTPUT(": ");
	for (int i = 0; attr[i]; i += 2)
	  OUTPUT(attr[i] << " = " << attr[i + 1] << "	");
      }
#endif



  XmlStartElementEvent *ev;
  NEW(ev, XmlStartElementEvent(name, attr));

  exposeEvent(ev);
}



/*!
  Examines the name of the element and emits corresponding event to the XML model.

  \param userData Pointer to the user data structure.
  \param name Name of the element.

  \sa XmlParser::exposeEvent()
 */
void XmlParser::endElement(void *userData,
			   const XmlChar *name)
{
  ParseInfo *parseInfo = (ParseInfo *)userData;


  parseInfo->depth--;

#ifdef PRINT_SAX  
  XmlChar *ee;
  NEW(ee, XmlChar[xmlchar_strlen(name)+10]);

  xmlchar_strcpy(ee, "</");
  xmlchar_strcat(ee, name);
  xmlchar_strcat(ee, ">");
  OUTPUT(ee);

  DELETE_ARRAY(ee);
#endif

  XmlEndElementEvent *ev;
  NEW(ev, XmlEndElementEvent(name));

  exposeEvent(ev);
}



/*!
  \brief Handler for characterData SAX event.

  Examines the character data and emits an event to the XML model. If the data contain characters \&, ', ", \< or \>, these are substituted by corresponding entities (\&amp;, \&apos;, \&quot;, \&lt; or \&gt;).

  \param userData Pointer to the user data structure.
  \param data Character data.
  \param length Length of the data.

  \sa XmlParser::exposeEvent()
 */
void XmlParser::characterData(void *userData,
			      const XmlChar *data,
			      int length)
{
#ifdef PRINT_SAX
  for (int i = 0; i < length; i++)
    OUTPUT(data[i]);
#endif

  XmlCharactersEvent *ev;
  NEW(ev, XmlCharactersEvent(data, length));

  exposeEvent(ev);

  charactersTotal += length;
}



/*!
  Examines the description of the processing instruction and emits an event to the XML model.

  \param userData Pointer to the user data structure.
  \param target Target of the instruction.
  \param data Data of the instruction.

  \sa XmlParser::exposeEvent()
 */
void XmlParser::processingInstruction(void *userData,
				      const XmlChar *target,
				      const XmlChar *data)
{
#ifdef PRINT_SAX  
  OUTPUT(target << " = " << data);
#endif

  XmlPIEvent *ev;
  NEW(ev, XmlPIEvent(target, data));

  exposeEvent(ev);
}



/*!
  Sends a corresponding event to the XML model.

  \param userData Pointer to the user data structure.
  \param data The comment data.

  \sa XmlParser::exposeEvent()
 */
void XmlParser::comment(void *userData,
			const XmlChar *data)
{
#ifdef PRINT_SAX  
  OUTPUT(data);
#endif

  XmlCommentEvent *ev;
  NEW(ev, XmlCommentEvent(data));

  exposeEvent(ev);
}



/*!
  Sends a corresponding event to the XML model.

  \param userData Pointer to the user data structure.

  \sa XmlParser::exposeEvent()
*/
void XmlParser::startCDATASection(void *userData)
{
#ifdef PRINT_SAX  
  OUTPUT("start CDATA");
#endif

  XmlStartCDATAEvent *ev;
  NEW(ev, XmlStartCDATAEvent);

  exposeEvent(ev);
}



/*!
  Sends a corresponding event to the XML model.

  \param userData Pointer to the user data structure.

  \sa XmlParser::exposeEvent()
*/
void XmlParser::endCDATASection(void *userData)
{
#ifdef PRINT_SAX  
  OUTPUT("end CDATA");
#endif

  XmlEndCDATAEvent *ev;
  NEW(ev, XmlEndCDATAEvent);

  exposeEvent(ev);
}



/*!
  Sends a corresponding event to the XML model.

  \param userData Pointer to the user data structure.
  \param data The default data.
  \param length The length of the default data.

  \sa XmlParser::exposeEvent()
*/
void XmlParser::defaultHandler(void *userData,
			       const XmlChar *data,
			       int length)
{

#ifdef PRINT_SAX
  for (int i = 0; i < length; i++)
    OUTPUT(data[i]);
  OUTPUT("DEFAULT");
#endif

  XmlDefaultEvent *ev;
  NEW(ev, XmlDefaultEvent(data, length));

  exposeEvent(ev);
}



/*!
  If there is no text codec specified, the parser creates a default one. The it registers the conversion functions.

  \param unknownEncodingData The description of the encoding.
  \param name The name of the encoding
  \param info The Expat's info structure.

  \sa XmlParser::exposeEvent()
  \sa TextCodec
*/
int XmlParser::unknownEncoding(void *unknownEncodingData,
			       const XmlChar *name,
			       XML_Encoding *info)
{
#ifdef PRINT_SAX  
  OUTPUT("Unknown encoding: " << name);
#endif

  if (!textCodec)
    {
      //use default text codec, if none has been specified
      createDefaultTextCodec();
    }

  Encodings::MIB mib;

  try
    {
      mib = textCodec->getMIB(name);
    }
  catch (ExaltUnknownEncodingException)
    {
      //encoding not listed among textcodec's encodings
      return 0;
    }

  if (mib == Encodings::Unknown)
    {
      //encoding not listed among textcodec's encodings
      //catch block should handle this case, this test is
      //just for safety
      return 0;
    }
  

  if (!textCodec->isAbleToConvert(mib))
    {
      //textcodec knows the encoding, but is unable to convert it
      return 0;
    }
  else
    {
      //textcodec understands the encoding
      textCodec->fillInMapArray(info, mib);
      info->convert = convert_wrapper;
      info->release = release_wrapper;

      EncodingInfo *ei;
      NEW(ei, EncodingInfo);

      ei->textCodec = textCodec;
      ei->mib = mib;

      info->data = (void *)ei;

      return 1;
    }
}



/*!
  Sends a corresponding event to the XML model.

  \param userData Pointer to the user data structure.
  \param version The XML version.
  \param encoding The encoding of the document.
  \param standalone Is the document standalone?

  \sa XmlParser::exposeEvent()
*/
void XmlParser::xmlDecl(void *userData,
			const XmlChar *version,
			const XmlChar *encoding,
			int standalone)
{
#ifdef PRINT_SAX  
  OUTPUT("XML declaration: version = " << version << ", encoding = " << encoding << ", standalone = " << standalone);
#endif

  XmlDeclEvent *ev;
  NEW(ev, XmlDeclEvent(version, encoding, standalone));

  exposeEvent(ev);

}



/*!
  Sends a corresponding event to the XML model.

  \param userData Pointer to the user data structure.
  \param doctypeName The name of the doctype.
  \param systemId The system id.
  \param publicId The public id.
  \param hasInternalSubset Has the DTD an internal subset?

  \sa XmlParser::exposeEvent()
*/
void XmlParser::startDoctypeDecl(void *userData,
				 const XmlChar *doctypeName,
				 const XmlChar *systemId,
				 const XmlChar *publicId,
				 int hasInternalSubset)
{
#ifdef PRINT_SAX  
  OUTPUT("Start Doctype: doctypeName = " << doctypeName << ", systemId = " << systemId << ", publicId = " << publicId << ", has internal subset = " << hasInternalSubset);
#endif

  XmlStartDoctypeEvent *ev;
  NEW(ev, XmlStartDoctypeEvent(doctypeName, systemId, publicId, hasInternalSubset));

  exposeEvent(ev);
}



/*!
  Sends a corresponding event to the XML model.

  \param userData Pointer to the user data structure.

  \sa XmlParser::exposeEvent()
*/
void XmlParser::endDoctypeDecl(void *userData)
{
#ifdef PRINT_SAX  
  OUTPUTNL("End doctype");
#endif

  XmlEndDoctypeEvent *ev;
  NEW(ev, XmlEndDoctypeEvent());

  exposeEvent(ev);
}



/*!
  Sends a corresponding event to the XML model.

  \param userData Pointer to the user data structure.
  \param entityName The name of the entity.
  \param isParameterEntity Is the entity a parametric entity?
  \param value The value of the entity.
  \param valueLength The length of the value.
  \param base The base.
  \param systemId The system id.
  \param publicId The public id.
  \param notationName The name of the notation.

  \sa XmlParser::exposeEvent()
*/
void XmlParser::entityDecl(void *userData,
			   const XmlChar *entityName,
			   int isParameterEntity,
			   const XmlChar *value,
			   int valueLength,
			   const XmlChar *base,
			   const XmlChar *systemId,
			   const XmlChar *publicId,
			   const XmlChar *notationName)
{
#ifdef PRINT_SAX  
  OUTPUT("Entity declaration: name = " << entityName << ", isParameterEntity " << isParameterEntity << ", valueLength = " << valueLength << ", base = " << base << ", systemId = " << systemId << ", publicId = " << publicId << ", notationName = " << notationName);
#endif

  XmlEntityDeclEvent *ev;
  NEW(ev, XmlEntityDeclEvent(entityName, isParameterEntity, value, valueLength, /* base, */ systemId, publicId, notationName));

  exposeEvent(ev);
}


/*!
  Sends a corresponding event to the XML model.

  \param userData Pointer to the user data structure.
  \param notationName The name of the notation.
  \param base The base.
  \param systemId The system id.
  \param publicId The public id.

  \sa XmlParser::exposeEvent()
*/
void XmlParser::notationDecl(void *userData,
			     const XmlChar *notationName,
			     const XmlChar *base,
			     const XmlChar *systemId,
			     const XmlChar *publicId)
{
#ifdef PRINT_SAX  
  OUTPUT("Notation declaration: name = " << notationName << ", base = " << base << ", systemId = " << systemId << ", publicId = " << publicId);
#endif

  XmlNotationDeclEvent *ev;
  NEW(ev, XmlNotationDeclEvent(notationName, /* base, */ systemId, publicId));

  exposeEvent(ev);
}




/*!
  Sends a XML event to the XML model.

  \param ev The XML event.
 */
void XmlParser::exposeEvent(XmlModelEvent *ev)
{
  if (xmlModel)
    xmlModel->manageEvent(ev);
  else
    WRN("No XML model has been specified for the parser!");
}



