/***************************************************************************
    xmlcodec.h  -  Definition of XmlCodec class
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
  \file xmlcodec.h
  \brief Definition of XmlCodec class.
  
  This file contains the definition of XmlCodec class.
*/

#ifdef __GNUG__
# pragma interface
#endif


#ifndef XMLCODEC_H
#define XMLCODEC_H



#include "arithcodec.h"
#include "context.h"
#include "defs.h"
#include "exceptions.h"
#include "funneldevice.h"
#include "kygrammar.h"
#include "xmlcodecbase.h"
#include "xmlparser.h"
#include "xmlsimplemodel.h"
#include "xmladaptivemodel.h"


/*!
  \brief A XML encoding/decoding class.

  This class implements the XML data encoding and decoding operations.
 */
class XmlCodec : public XmlCodecBase
{
public:
  //! A constructor.
  XmlCodec(void);

  //! A destructor
  virtual ~XmlCodec(void);

  //! Encode data from one device to another (the PULL mode).
  virtual bool encode(IODevice *inDevice, IODevice *outDevice);

  //! Initialize the coder in PUSH mode.
  virtual void initializePushCoder(IODevice *outDevice);

  //! Encode the data in PUSH mode.
  virtual bool encodePush(const char *data, int length, bool isFinal = false);

  //! Decode data from one device and use given SAX receptor.
  virtual bool decode(IODevice *inDevice, SAXReceptor *receptor, void *userData = 0);



protected:
  //! The input device.
  IODevice *inputDevice;

  //! The output device.
  IODevice *outputDevice;

  //! The XML parser.
  XmlParser *xmlParser;

  //! The XML model.
  XmlModelBase *xmlModel;

  //! The KY grammar.
  KYGrammar *kyGrammar;

  //! The arithmetic coding context.
  Context *context;

  //! The arithmetic coder/decoder.
  ArithCodec *arithCodec;
};


#endif //XMLCODEC_H
