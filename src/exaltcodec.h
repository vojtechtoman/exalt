/***************************************************************************
    exaltcodec.h  -  Definition of ExaltCodec class
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
  \file exaltcodec.h
  \brief Definition of ExaltCodec class.
  
  This file contains the definition of ExaltCodec class.
*/

#ifdef __GNUG__
# pragma interface
#endif


#ifndef EXALTCODEC_H
#define EXALTCODEC_H


#include "defs.h"
#include "filedevice.h"
#include "iodevice.h"
#include "saxemitter.h"
#include "saxreceptor.h"
#include "outputsaxreceptor.h"
#include "textcodec.h"
#include "xmlcodec.h"
#include "xmlcodecbase.h"


#include <ctime>



/*!
  \brief Convenience class for easy XML encoding/decoding.

  ExaltCodec offers a simple interface for encoding and decoding XML data.
 */
class ExaltCodec : public XmlCodecBase
{
public:
  //! A constructor.
  ExaltCodec(void);

  //! A destructor.
  virtual ~ExaltCodec(void);

  //! Encode data in specified file.
  virtual bool encode(const char *inFileName, const char *outFileName);

  //! Encode data in specified input device.
  virtual bool encode(IODevice *inDevice, IODevice *outDevice);

  //! Initialize the coder in PUSH mode (using specified output device).
  virtual void initializePushCoder(IODevice *outDevice);

  //! Initialize the coder in PUSH mode (using specified output file).
  virtual void initializePushCoder(const char *outFileName);

  //! Encode a chunk of XML data in PUSH mode.
  virtual bool encodePush(const char *data, int length, bool isFinal = false);


  //! Decode data from specified file.
  virtual bool decode(const char *inFileName, const char *outFileName);

  //! Decode data from specified input device.
  virtual bool decode(IODevice *inDevice, IODevice *outDevice);

  //! Decode data from specified input device and use SAX event receptor.
  virtual bool decode(IODevice *inDevice, SAXReceptor *receptor, void *userData = 0);

  //! Decode data from specified file and use SAX event receptor.
  virtual bool decode(const char *inFileName, SAXReceptor *receptor, void *userData = 0);

protected:
  //! The instance of the XmlCodec.
  XmlCodecBase *xmlCodec;
}; //ExaltCodec


#endif //EXALTCODEC_H
