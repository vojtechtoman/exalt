/***************************************************************************
    xmlparserbase.h  -  Definition of XmlParserBase class
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
  \file xmlparserbase.h
  \brief Definition of the XmlParserBase class.
  
  This file contains definition of the XmlParserBase class.
*/



#ifndef XMLPARSERBASE_H
#define XMLPARSERBASE_H

#include "defs.h"
#include "iodevice.h"
#include "xmldefs.h"
#include "xmlmodelbase.h"
#include "saxbase.h"

/*!
  \brief Base class for all XML parsers.

  This class implements a simple Expat-based XML parser. It defines all the basic handlers (which are mostly doing nothing) and serves as a base class for inhering of new parsers.
 */
class XmlParserBase : public SAXBase, public UserOfTextCodec
{
public:
  /*!
    \brief A constructor.

    Calls init_().
  */
  XmlParserBase(void)
    : SAXBase(), UserOfTextCodec()
  {
    init_();
  }


  /*!
    \brief A constructor with an encoding.

    \param encoding The name of the encoding (IGNORED).
   */
  XmlParserBase(const XmlChar *encoding)
    : SAXBase(), UserOfTextCodec()
  {
    init_();
  }

  /*!
    \brief A destructor.

    Does nothing.
   */
  virtual ~XmlParserBase(void) {}

  /*!
    \brief Set input device.

    Sets an input device for the parser. XML data stream is supposed to come from this device.

    \param dev Pointer to the input device.
   */
  virtual void setInputDevice(IODevice *dev) { inputDevice = dev; }

  /*!
    \brief Set XML model.

    All XML events (NOT SAX events which are handled by the parser itself) that are generated during parsing, are sent to this model.

    \param model Pointer to the XML model.

    \sa XmlModelEvent
    \sa XmlModel
   */
  virtual void setXmlModel(XmlModelBase *model) { xmlModel = model; }

  /*!
    \brief Parse XML data stream.

    \retval 0 Error during parsing.
    \return Number of parsed bytes otherwise.
   */
  virtual long parse(void) throw (ExaltParserException, ExaltEncodingException, ExaltIOException)
  {
    return true; 
  }

  /*!
    \brief Parse XML data in a buffer (PUSH parser).

    Always returns \a true.

    \retval 0 Error during parsing.
    \return Number of parsed bytes otherwise.
   */
  virtual long parsePush(const char *data, int length, bool isFinal) throw (ExaltParserException, ExaltEncodingException, ExaltIOException)
  {
    return true; 
  }


protected:
  //! The input device.
  IODevice *inputDevice;

  //! The XML model.
  XmlModelBase *xmlModel;

  /*!
    \brief Initialize the parser.

    Initializes the member variables of the class.
   */
  void init_(void)
  {
    inputDevice = 0;
    xmlModel = 0;
  }
};




#endif //XMLPARSERBASE_H
