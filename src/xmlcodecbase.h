/***************************************************************************
    xmlcodecbase.h  -  Definition of XmlCodecBase class
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
  \file xmlcodecbase.h
  \brief Definition of XmlCodecBase class.
  
  This file contains the definition of XmlCodecBase class.
*/




#ifndef XMLCODECBASE_H
#define XMLCODECBASE_H


#include "defs.h"
#include "options.h"
#include "iodevice.h"
#include "textcodec.h"
#include "saxreceptor.h"



//! The possible types of the coder.
enum XmlCoderType
{
  //! An unknown coder.
  UnknownCoder,

  //! A PULL coder.
  PullCoder,

  //! A PUSH coder.
  PushCoder
};




/*!
  \brief An abstract predecessor of all XML codecs.

  This class implements a simple and unified interface for all XMl coding related classes.
 */
class XmlCodecBase : public UserOfTextCodec
{
public:
  //! A constructor.
  XmlCodecBase(void) : UserOfTextCodec() { coderType = UnknownCoder; }

  /*!
    \brief A destructor.

    Does nothing.
   */
  virtual ~XmlCodecBase(void) {}
  
  /*!
    \brief Encode data from one device to another (the PULL mode).
    
    \param inDevice The input device.
    \param outDevice The output device.
   */
  virtual bool encode(IODevice *inDevice, IODevice *outDevice) = 0;

  /*!
    \brief Initialize the coder in PUSH mode.
    \param outDevice The output device.
   */
  virtual void initializePushCoder(IODevice *outDevice) = 0;

  /*!
    \brief Encode the data in PUSH mode.

    The coder encodes given chunk of data.

    \param data The chunk of XML data.
    \param length The length of the data.
    \param isFinal Indication whether the coder received the last chunk of data.
   */
  virtual bool encodePush(const char *data, int length, bool isFinal = false) = 0;

  /*!
    \brief Decode data from one device and use given SAX receptor.

    \param inDevice The input device.
    \param receptor The SAX event receptor.
    \param userData The user data structure.
   */
  virtual bool decode(IODevice *inDevice, SAXReceptor *receptor, void *userData = 0) = 0;

protected:
  //! The type of the coder.
  XmlCoderType coderType;
};


#endif //XMLCODECBASE_H
